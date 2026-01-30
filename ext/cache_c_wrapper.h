#ifndef APM_PHP_EXT_CACHE_C_WRAPPER_H
#define APM_PHP_EXT_CACHE_C_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Zend/zend.h>

/**
 * Allocate and initialize a new cache instance.
 *
 * @param cache_max_entries The maximum number of entries the cache can hold.
 * @return A pointer to the allocated cache instance.
 */
void *Cache_Allocate(long cache_max_entries);

/**
 * Free the allocated cache instance.
 *
 * @param cache A pointer to the cache instance to be freed.
 */
void Cache_Free(void *cache);

/**
 * Store settings in the cache.
 *
 * @param cache A pointer to the cache instance.
 * @param collector The collector endpoint.
 * @param token The token.
 * @param serviceName The name of the service.
 * @param settings The settings to be cached.
 */
void Cache_Put(void *cache, char *collector, char *token, char *serviceName,
               char *settings);

/**
 * Retrieve settings from the cache.
 * @param cache A pointer to the cache instance.
 * @param collector The collector endpoint.
 * @param token The token.
 * @param serviceName The name of the service.
 * @return The cached settings if found, otherwise NULL.
 */
zend_string *Cache_Get(void *cache, char *collector, char *token,
                       char *serviceName);

#ifdef __cplusplus
}
#endif

#endif // APM_PHP_EXT_CACHE_C_WRAPPER_H
