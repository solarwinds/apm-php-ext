#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h" /* zend_try/zend_catch need the full PHP/Zend config */
#include "cache_c_wrapper.h"
#include "cache.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Max decimal digits of a 64-bit unsigned integer. */
#define HASH_MAX_DIGITS 20

/*
 * Convert a cache snapshot to a zend_string and free the snapshot. Called
 * after the cache lock is released; the snapshot is freed even if
 * zend_string_init() bails out (e.g. memory_limit exceeded).
 */
static zend_string *snapshot_to_zend_string(char *snapshot, size_t len) {
  char *volatile snap = snapshot;
  zend_string *volatile result = NULL;

  if (snap == NULL) {
    return NULL;
  }
  zend_try { result = zend_string_init(snap, len, 0); }
  zend_catch {
    sw_cache_value_free(snap);
    zend_bailout();
  }
  zend_end_try();
  sw_cache_value_free(snap);
  return result;
}

/*
 * Build the settings key `collector + hash(token) + serviceName` so the raw
 * token is never stored. Returns a malloc'd buffer (caller frees) or NULL.
 */
static char *build_settings_key(const char *collector, const char *token,
                                const char *serviceName, size_t *key_len) {
  char hash_buf[HASH_MAX_DIGITS + 1];
  size_t collector_len = strlen(collector);
  size_t service_len = strlen(serviceName);
  int hash_len = snprintf(hash_buf, sizeof(hash_buf), "%" PRIu64,
                          (uint64_t)sw_cache_hash(token, strlen(token)));
  char *key;

  if (hash_len < 0) {
    return NULL;
  }
  *key_len = collector_len + (size_t)hash_len + service_len;
  key = (char *)malloc(*key_len + 1);
  if (key == NULL) {
    return NULL;
  }
  memcpy(key, collector, collector_len);
  memcpy(key + collector_len, hash_buf, (size_t)hash_len);
  memcpy(key + collector_len + hash_len, serviceName, service_len);
  key[*key_len] = '\0';
  return key;
}

void *Cache_Allocate(long max_entries) {
  return sw_cache_create(max_entries > 0 ? (size_t)max_entries : 0);
}

void Cache_Free(void *cache) { sw_cache_destroy((sw_cache *)cache); }

void Cache_Put(void *cache, char *collector, char *token, char *serviceName,
               char *settings) {
  if (cache != NULL && collector != NULL && token != NULL &&
      serviceName != NULL && settings != NULL) {
    size_t key_len;
    char *key = build_settings_key(collector, token, serviceName, &key_len);
    if (key != NULL) {
      sw_cache_put((sw_cache *)cache, key, key_len, settings,
                   strlen(settings));
      free(key);
    }
  }
}

zend_string *Cache_Get(void *cache, char *collector, char *token,
                       char *serviceName) {
  char *snapshot = NULL;
  size_t len = 0;
  if (cache != NULL && collector != NULL && token != NULL &&
      serviceName != NULL) {
    size_t key_len;
    char *key = build_settings_key(collector, token, serviceName, &key_len);
    if (key != NULL) {
      snapshot = sw_cache_get((sw_cache *)cache, key, key_len, &len);
      free(key); /* before any Zend allocation that could bail out */
    }
  }
  return snapshot_to_zend_string(snapshot, len);
}

void Cache_PutBucketState(void *cache, char *pid, char *bucketTokenState) {
  if (cache != NULL && pid != NULL && bucketTokenState != NULL) {
    sw_cache_put((sw_cache *)cache, pid, strlen(pid), bucketTokenState,
                 strlen(bucketTokenState));
  }
}

zend_string *Cache_GetBucketState(void *cache, char *pid) {
  if (cache != NULL && pid != NULL) {
    size_t len = 0;
    char *snapshot = sw_cache_get((sw_cache *)cache, pid, strlen(pid), &len);
    return snapshot_to_zend_string(snapshot, len);
  }
  return NULL;
}
