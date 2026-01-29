#include "cache_c_wrapper.h"
#include "cache.h"
#include <new>

void *Cache_Allocate(long cache_max_size) {
  return new (std::nothrow) Solarwinds::Cache(cache_max_size);
}

void Cache_Free(void *cache) {
  if (cache != nullptr) {
    auto s = static_cast<Solarwinds::Cache *>(cache);
    delete s;
  }
}

void Cache_Put(void *cache, char *collector, char *token, char *serviceName,
               char *settings) {
  if (cache != nullptr && collector != nullptr && token != nullptr &&
      serviceName != nullptr && settings != nullptr) {
    auto c = static_cast<Solarwinds::Cache *>(cache);
    c->Put(std::string(collector), std::string(token), std::string(serviceName),
           std::string(settings));
  }
}

zend_string *Cache_Get(void *cache, char *collector, char *token,
                       char *serviceName) {
  if (cache != nullptr && collector != nullptr && token != nullptr &&
      serviceName != nullptr) {
    auto c = static_cast<Solarwinds::Cache *>(cache);
    auto result = c->Get(std::string(collector), std::string(token),
                         std::string(serviceName));
    if (result.first) {
      return zend_string_init(result.second.c_str(), result.second.size(),
                              false);
    }
  }
  return nullptr;
}
