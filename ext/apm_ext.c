/* apm_ext extension for PHP */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_apm_ext.h"
#include "cache.h"
#include "apm_ext_arginfo.h"
#ifndef _WIN32
#include <pthread.h>
#endif
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE()                                           \
  ZEND_PARSE_PARAMETERS_START(0, 0)                                            \
  ZEND_PARSE_PARAMETERS_END()
#endif

#define COLLECTOR_MAX_LENGTH 100
#define TOKEN_MAX_LENGTH 128
#define SERVICE_NAME_MAX_LENGTH 128
// Windows process ID range can be [0, 4294967295]
#define PID_MAX_LENGTH 10

ZEND_DECLARE_MODULE_GLOBALS(apm_ext)

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("apm_ext.cache_max_entries", "48", PHP_INI_ALL,
                  OnUpdateLongGEZero, cache_max_entries, zend_apm_ext_globals,
                  apm_ext_globals)
STD_PHP_INI_ENTRY("apm_ext.settings_max_length", "2048", PHP_INI_ALL,
                  OnUpdateLongGEZero, settings_max_length, zend_apm_ext_globals,
                  apm_ext_globals)
STD_PHP_INI_ENTRY("apm_ext.bucket_state_cache_max_entries", "512", PHP_INI_ALL,
                  OnUpdateLongGEZero, bucket_state_cache_max_entries, zend_apm_ext_globals,
                  apm_ext_globals)
STD_PHP_INI_ENTRY("apm_ext.bucket_state_max_length", "2048", PHP_INI_ALL,
                  OnUpdateLongGEZero, bucket_state_max_length, zend_apm_ext_globals,
                  apm_ext_globals)
PHP_INI_END()

/* Max decimal digits of a 64-bit unsigned integer. */
#define HASH_MAX_DIGITS 20

/*
 * Lengths below come from strlen(), so a PHP string containing a NUL byte is
 * treated as ending at that byte (the behaviour of the original C++ bridge).
 */

/*
 * Build the settings key `collector + hash(token) + serviceName` so the raw
 * token is never stored. Returns a malloc'd buffer (caller frees) or NULL.
 */
static char *build_settings_key(const char *collector, const char *token,
                                const char *service_name, size_t *key_len) {
  char hash_buf[HASH_MAX_DIGITS + 1];
  size_t collector_len = strlen(collector);
  size_t service_len = strlen(service_name);
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
  memcpy(key + collector_len + hash_len, service_name, service_len);
  key[*key_len] = '\0';
  return key;
}

/*
 * Convert a cache snapshot to a zend_string and free the snapshot. Must be
 * called after the cache lock is released; the snapshot is freed even if
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

static sw_cache *cache_allocate(zend_long max_entries) {
  return sw_cache_create(max_entries > 0 ? (size_t)max_entries : 0);
}

/* {{{ void Solarwinds\\Cache\\get() */
PHP_FUNCTION(Solarwinds_Cache_get) {
  char *collector;
  size_t collector_len;
  char *token;
  size_t token_len;
  char *service_name;
  size_t service_name_len;

  ZEND_PARSE_PARAMETERS_START(3, 3)
  Z_PARAM_STRING(collector, collector_len)
  Z_PARAM_STRING(token, token_len)
  Z_PARAM_STRING(service_name, service_name_len)
  ZEND_PARSE_PARAMETERS_END();

  if (!collector_len || !token_len || !service_name_len) {
    RETURN_FALSE;
  }

  size_t key_len;
  char *key = build_settings_key(collector, token, service_name, &key_len);
  if (key == NULL) {
    RETURN_FALSE;
  }
  size_t settings_len = 0;
  char *snapshot =
      sw_cache_get(APM_EXT_G(cache), key, key_len, &settings_len);
  free(key); /* before any Zend allocation that could bail out */

  zend_string *settings = snapshot_to_zend_string(snapshot, settings_len);
  if (settings != NULL) {
    RETURN_NEW_STR(settings);
  }
  RETURN_FALSE;
}
/* }}} */

/* {{{ void Solarwinds\\Cache\\put() */
PHP_FUNCTION(Solarwinds_Cache_put) {
  char *collector;
  size_t collector_len;
  char *token;
  size_t token_len;
  char *service_name;
  size_t service_name_len;
  char *settings;
  size_t settings_len;
  ZEND_PARSE_PARAMETERS_START(4, 4)
  Z_PARAM_STRING(collector, collector_len)
  Z_PARAM_STRING(token, token_len)
  Z_PARAM_STRING(service_name, service_name_len)
  Z_PARAM_STRING(settings, settings_len)
  ZEND_PARSE_PARAMETERS_END();

  if (collector_len && token_len && service_name_len && settings_len) {
    if (collector_len > COLLECTOR_MAX_LENGTH) {
      fprintf(stderr, "apm_ext: collector length %zu exceeds max length %d\n",
              collector_len, COLLECTOR_MAX_LENGTH);
      RETURN_FALSE;
    }
    if (token_len > TOKEN_MAX_LENGTH) {
      fprintf(stderr, "apm_ext: token length %zu exceeds max length %d\n",
              token_len, TOKEN_MAX_LENGTH);
      RETURN_FALSE;
    }
    if (service_name_len > SERVICE_NAME_MAX_LENGTH) {
      fprintf(stderr,
              "apm_ext: service name length %zu exceeds max length %d\n",
              service_name_len, SERVICE_NAME_MAX_LENGTH);
      RETURN_FALSE;
    }
    if (settings_len > (size_t)APM_EXT_G(settings_max_length)) {
      fprintf(stderr, "apm_ext: settings length %zu exceeds max length %ld\n",
              settings_len, (long)APM_EXT_G(settings_max_length));
      RETURN_FALSE;
    }
    size_t key_len;
    char *key = build_settings_key(collector, token, service_name, &key_len);
    if (key == NULL) {
      RETURN_FALSE;
    }
    int ok = sw_cache_put(APM_EXT_G(cache), key, key_len, settings,
                          strlen(settings));
    free(key);
    RETURN_BOOL(ok);
  }
  RETURN_FALSE;
}
/* }}} */

/* {{{ void Solarwinds\\Cache\\getBucketState() */
PHP_FUNCTION(Solarwinds_Cache_getBucketState) {
  char *pid;
  size_t pid_len;

  ZEND_PARSE_PARAMETERS_START(1, 1)
  Z_PARAM_STRING(pid, pid_len)
  ZEND_PARSE_PARAMETERS_END();

  if (!pid_len) {
    RETURN_FALSE;
  }

  size_t state_len = 0;
  char *snapshot = sw_cache_get(APM_EXT_G(bucket_state_cache), pid,
                                strlen(pid), &state_len);
  zend_string *state = snapshot_to_zend_string(snapshot, state_len);
  if (state != NULL) {
    RETURN_NEW_STR(state);
  }
  RETURN_FALSE;
}
/* }}} */

/* {{{ void Solarwinds\\Cache\\putBucketState() */
PHP_FUNCTION(Solarwinds_Cache_putBucketState) {
  char *pid;
  size_t pid_len;
  char *bucket_state;
  size_t bucket_state_len;

  ZEND_PARSE_PARAMETERS_START(2, 2)
  Z_PARAM_STRING(pid, pid_len)
  Z_PARAM_STRING(bucket_state, bucket_state_len)
  ZEND_PARSE_PARAMETERS_END();

  if (pid_len && bucket_state_len) {
    if (pid_len > PID_MAX_LENGTH) {
      fprintf(stderr, "apm_ext: pid length %zu exceeds max length %d\n", pid_len, PID_MAX_LENGTH);
      RETURN_FALSE;
    }
    if (bucket_state_len > (size_t)APM_EXT_G(bucket_state_max_length)) {
      fprintf(stderr, "apm_ext: bucket state length %zu exceeds max length %ld\n",
              bucket_state_len, (long)APM_EXT_G(bucket_state_max_length));
      RETURN_FALSE;
    }
    RETURN_BOOL(sw_cache_put(APM_EXT_G(bucket_state_cache), pid, strlen(pid),
                             bucket_state, strlen(bucket_state)));
  }
  RETURN_FALSE;
}
/* }}} */

#ifndef _WIN32
void prefork() {
  sw_cache_destroy(APM_EXT_G(bucket_state_cache));
  APM_EXT_G(bucket_state_cache) = NULL;
  sw_cache_destroy(APM_EXT_G(cache));
  APM_EXT_G(cache) = NULL;
}

void postfork() {
  APM_EXT_G(cache) = cache_allocate(APM_EXT_G(cache_max_entries));
  APM_EXT_G(bucket_state_cache) = cache_allocate(APM_EXT_G(bucket_state_cache_max_entries));
}
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(apm_ext) {
#if defined(ZTS) && defined(COMPILE_DL_APM_EXT)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  REGISTER_INI_ENTRIES();

  APM_EXT_G(cache) = cache_allocate(APM_EXT_G(cache_max_entries));
  APM_EXT_G(bucket_state_cache) = cache_allocate(APM_EXT_G(bucket_state_cache_max_entries));
#ifndef _WIN32
  pthread_atfork(prefork, postfork, postfork);
#endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(apm_ext) {
  sw_cache_destroy(APM_EXT_G(bucket_state_cache));
  APM_EXT_G(bucket_state_cache) = NULL;
  sw_cache_destroy(APM_EXT_G(cache));
  APM_EXT_G(cache) = NULL;

  UNREGISTER_INI_ENTRIES();

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(apm_ext) {
#if defined(ZTS) && defined(COMPILE_DL_APM_EXT)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(apm_ext) {
  php_info_print_table_start();
  php_info_print_table_header(2, "apm_ext support", "enabled");
  php_info_print_table_end();
  DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
PHP_GINIT_FUNCTION(apm_ext) {
  ZEND_SECURE_ZERO(apm_ext_globals, sizeof(*apm_ext_globals));
}
/* }}} */

/* {{{ apm_ext_module_entry */
zend_module_entry apm_ext_module_entry = {
    STANDARD_MODULE_HEADER,
    "apm_ext",              /* Extension name */
    ext_functions,          /* zend_function_entry */
    PHP_MINIT(apm_ext),     /* PHP_MINIT - Module initialization */
    PHP_MSHUTDOWN(apm_ext), /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(apm_ext),     /* PHP_RINIT - Request initialization */
    NULL,                   /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(apm_ext),     /* PHP_MINFO - Module info */
    PHP_APM_EXT_VERSION,    /* Version */
    PHP_MODULE_GLOBALS(apm_ext),
    PHP_GINIT(apm_ext),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX};
/* }}} */

#ifdef COMPILE_DL_APM_EXT
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(apm_ext)
#endif
