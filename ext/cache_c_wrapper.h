#ifndef APM_PHP_EXT_CACHE_C_WRAPPER_H
#define APM_PHP_EXT_CACHE_C_WRAPPER_H

#define SETTINGS_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Allocate a Solarwinds::Cache object
     * @return void*
     */
    void *Cache_Allocate();

    /**
     * Free a Solarwinds::Cache object
     * @param cache void*
     * @return
     */
    void Cache_Free(void *cache);

    void Cache_Put(void* cache, char* collector, char* token, char* serviceName, char* settings);

    bool Cache_Get(void* cache, char* collector, char* token, char* serviceName, char* ans);

#ifdef __cplusplus
}
#endif

#endif //APM_PHP_EXT_CACHE_C_WRAPPER_H
