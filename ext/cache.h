#ifndef APM_PHP_EXT_CACHE_H
#define APM_PHP_EXT_CACHE_H

#include <stddef.h>

/**
 * Simple thread-safe in-memory LRU cache mapping byte-string keys to
 * byte-string values.
 *
 * Both Get and Put move the entry to the most-recently-used position. When a
 * new key is inserted and the cache is full, the least-recently-used entry is
 * evicted. A max_entries of 0 means the cache is unbounded.
 *
 * Memory is allocated with the system allocator (not the Zend request
 * allocator) so entries survive across requests.
 */
typedef struct sw_cache sw_cache;

/**
 * Non-cryptographic 64-bit FNV-1a hash of a byte string, truncated to size_t.
 */
size_t sw_cache_hash(const char *data, size_t len);

/**
 * Allocate a new cache.
 *
 * @param max_entries The maximum number of entries the cache can hold.
 * @return The new cache, or NULL on allocation failure.
 */
sw_cache *sw_cache_create(size_t max_entries);

/**
 * Free the cache and all its entries. NULL is a no-op.
 */
void sw_cache_destroy(sw_cache *cache);

/**
 * Insert or update an entry.
 *
 * @return 1 on success, 0 on allocation failure (the cache is unchanged).
 */
int sw_cache_put(sw_cache *cache, const char *key, size_t key_len,
                 const char *value, size_t value_len);

/**
 * Look up an entry and invoke `copy` with its value while the cache lock is
 * held, so the caller can copy it into its own storage.
 *
 * @return The result of `copy` if found, otherwise NULL.
 */
void *sw_cache_get(sw_cache *cache, const char *key, size_t key_len,
                   void *(*copy)(const char *value, size_t value_len));

#endif // APM_PHP_EXT_CACHE_H
