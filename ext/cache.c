#include "cache.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
typedef SRWLOCK sw_mutex;
#define SW_MUTEX_INIT(m) (InitializeSRWLock(m), 1)
#define SW_MUTEX_DESTROY(m) ((void)(m))
#define SW_MUTEX_LOCK(m) AcquireSRWLockExclusive(m)
#define SW_MUTEX_UNLOCK(m) ReleaseSRWLockExclusive(m)
#else
#include <pthread.h>
typedef pthread_mutex_t sw_mutex;
#define SW_MUTEX_INIT(m) (pthread_mutex_init((m), NULL) == 0)
#define SW_MUTEX_DESTROY(m) pthread_mutex_destroy(m)
#define SW_MUTEX_LOCK(m) pthread_mutex_lock(m)
#define SW_MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#endif

#define SW_CACHE_MIN_BUCKETS 16

typedef struct sw_cache_entry {
  /* LRU list: head is least recently used, tail is most recently used. */
  struct sw_cache_entry *prev;
  struct sw_cache_entry *next;
  /* Hash bucket chain. */
  struct sw_cache_entry *chain;
  size_t hash;
  char *key;
  size_t key_len;
  char *value;
  size_t value_len;
} sw_cache_entry;

struct sw_cache {
  size_t max_entries; /* 0 = unbounded */
  size_t size;
  size_t bucket_count; /* always a power of two */
  sw_cache_entry **buckets;
  sw_cache_entry *head;
  sw_cache_entry *tail;
  sw_mutex mutex;
};

size_t sw_cache_hash(const char *data, size_t len) {
  uint64_t h = UINT64_C(14695981039346656037);
  size_t i;
  for (i = 0; i < len; i++) {
    h ^= (unsigned char)data[i];
    h *= UINT64_C(1099511628211);
  }
  return (size_t)h;
}

static char *sw_memdup(const char *src, size_t len) {
  /* +1 keeps values NUL-terminated for convenience; len is authoritative. */
  char *dst = (char *)malloc(len + 1);
  if (dst != NULL) {
    if (len) {
      memcpy(dst, src, len);
    }
    dst[len] = '\0';
  }
  return dst;
}

static void sw_entry_free(sw_cache_entry *e) {
  free(e->key);
  free(e->value);
  free(e);
}

static void sw_list_unlink(sw_cache *c, sw_cache_entry *e) {
  if (e->prev) {
    e->prev->next = e->next;
  } else {
    c->head = e->next;
  }
  if (e->next) {
    e->next->prev = e->prev;
  } else {
    c->tail = e->prev;
  }
  e->prev = e->next = NULL;
}

static void sw_list_push_tail(sw_cache *c, sw_cache_entry *e) {
  e->prev = c->tail;
  e->next = NULL;
  if (c->tail) {
    c->tail->next = e;
  } else {
    c->head = e;
  }
  c->tail = e;
}

static void sw_list_move_to_tail(sw_cache *c, sw_cache_entry *e) {
  if (c->tail != e) {
    sw_list_unlink(c, e);
    sw_list_push_tail(c, e);
  }
}

static sw_cache_entry **sw_bucket_slot(sw_cache *c, size_t hash) {
  return &c->buckets[hash & (c->bucket_count - 1)];
}

static sw_cache_entry *sw_find(sw_cache *c, const char *key, size_t key_len,
                               size_t hash) {
  sw_cache_entry *e = *sw_bucket_slot(c, hash);
  for (; e != NULL; e = e->chain) {
    if (e->hash == hash && e->key_len == key_len &&
        memcmp(e->key, key, key_len) == 0) {
      return e;
    }
  }
  return NULL;
}

static void sw_bucket_remove(sw_cache *c, sw_cache_entry *e) {
  sw_cache_entry **slot = sw_bucket_slot(c, e->hash);
  while (*slot != e) {
    slot = &(*slot)->chain;
  }
  *slot = e->chain;
  e->chain = NULL;
}

static void sw_bucket_insert(sw_cache *c, sw_cache_entry *e) {
  sw_cache_entry **slot = sw_bucket_slot(c, e->hash);
  e->chain = *slot;
  *slot = e;
}

static void sw_evict_lru(sw_cache *c) {
  sw_cache_entry *e = c->head;
  if (e != NULL) {
    sw_list_unlink(c, e);
    sw_bucket_remove(c, e);
    sw_entry_free(e);
    c->size--;
  }
}

/* Grow the bucket array to keep load factor <= 1. Best effort: on allocation
 * failure the cache keeps working with longer chains. */
static void sw_maybe_grow(sw_cache *c) {
  size_t new_count, i;
  sw_cache_entry **new_buckets;
  if (c->size < c->bucket_count || c->bucket_count > (SIZE_MAX >> 1)) {
    return;
  }
  new_count = c->bucket_count << 1;
  new_buckets = (sw_cache_entry **)calloc(new_count, sizeof(*new_buckets));
  if (new_buckets == NULL) {
    return;
  }
  for (i = 0; i < c->bucket_count; i++) {
    sw_cache_entry *e = c->buckets[i];
    while (e != NULL) {
      sw_cache_entry *next = e->chain;
      sw_cache_entry **slot = &new_buckets[e->hash & (new_count - 1)];
      e->chain = *slot;
      *slot = e;
      e = next;
    }
  }
  free(c->buckets);
  c->buckets = new_buckets;
  c->bucket_count = new_count;
}

sw_cache *sw_cache_create(size_t max_entries) {
  size_t buckets = SW_CACHE_MIN_BUCKETS;
  sw_cache *c = (sw_cache *)calloc(1, sizeof(*c));
  if (c == NULL) {
    return NULL;
  }
  /* Pre-size for bounded caches so no rehash is needed in steady state. */
  while (buckets < max_entries && buckets <= (SIZE_MAX >> 1) / sizeof(void *)) {
    buckets <<= 1;
  }
  c->buckets = (sw_cache_entry **)calloc(buckets, sizeof(*c->buckets));
  if (c->buckets == NULL) {
    free(c);
    return NULL;
  }
  if (!SW_MUTEX_INIT(&c->mutex)) {
    free(c->buckets);
    free(c);
    return NULL;
  }
  c->bucket_count = buckets;
  c->max_entries = max_entries;
  return c;
}

void sw_cache_destroy(sw_cache *c) {
  sw_cache_entry *e;
  if (c == NULL) {
    return;
  }
  e = c->head;
  while (e != NULL) {
    sw_cache_entry *next = e->next;
    sw_entry_free(e);
    e = next;
  }
  free(c->buckets);
  SW_MUTEX_DESTROY(&c->mutex);
  free(c);
}

int sw_cache_put(sw_cache *c, const char *key, size_t key_len,
                 const char *value, size_t value_len) {
  sw_cache_entry *e, *existing;

  if (c == NULL || key == NULL || value == NULL) {
    return 0;
  }

  /* Allocate outside the lock. */
  e = (sw_cache_entry *)calloc(1, sizeof(*e));
  if (e == NULL) {
    return 0;
  }
  e->key = sw_memdup(key, key_len);
  e->value = sw_memdup(value, value_len);
  if (e->key == NULL || e->value == NULL) {
    sw_entry_free(e);
    return 0;
  }
  e->key_len = key_len;
  e->value_len = value_len;
  e->hash = sw_cache_hash(key, key_len);

  SW_MUTEX_LOCK(&c->mutex);
  existing = sw_find(c, key, key_len, e->hash);
  if (existing != NULL) {
    /* update: swap in the new value, move to MRU */
    char *old_value = existing->value;
    existing->value = e->value;
    existing->value_len = e->value_len;
    e->value = old_value;
    sw_list_move_to_tail(c, existing);
    SW_MUTEX_UNLOCK(&c->mutex);
    sw_entry_free(e); /* frees the unused key copy and the old value */
    return 1;
  }
  /* new: evict LRU entries until there is room */
  if (c->max_entries > 0) {
    while (c->size >= c->max_entries) {
      sw_evict_lru(c);
    }
  }
  sw_bucket_insert(c, e);
  sw_list_push_tail(c, e);
  c->size++;
  sw_maybe_grow(c);
  SW_MUTEX_UNLOCK(&c->mutex);
  return 1;
}

void *sw_cache_get(sw_cache *c, const char *key, size_t key_len,
                   void *(*copy)(const char *value, size_t value_len)) {
  void *result = NULL;
  sw_cache_entry *e;
  size_t hash;

  if (c == NULL || key == NULL || copy == NULL) {
    return NULL;
  }
  hash = sw_cache_hash(key, key_len);

  SW_MUTEX_LOCK(&c->mutex);
  e = sw_find(c, key, key_len, hash);
  if (e != NULL) {
    sw_list_move_to_tail(c, e);
    result = copy(e->value, e->value_len);
  }
  SW_MUTEX_UNLOCK(&c->mutex);
  return result;
}
