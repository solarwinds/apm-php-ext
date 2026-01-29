#ifndef APM_PHP_EXT_CACHE_C_WRAPPER_H
#define APM_PHP_EXT_CACHE_C_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Zend/zend.h>

/**
 * Allocate a Solarwinds::Cache object
 * @return void*
 */
void *Cache_Allocate(long cache_max_entries);

/**
 * Free a Solarwinds::Cache object
 * @param cache void*
 * @return
 */
void Cache_Free(void *cache);

void Cache_Put(void *cache, char *collector, char *token, char *serviceName,
               char *settings);

zend_string *Cache_Get(void *cache, char *collector, char *token,
                       char *serviceName);

#ifdef __cplusplus
}
#endif

#endif // APM_PHP_EXT_CACHE_C_WRAPPER_H
