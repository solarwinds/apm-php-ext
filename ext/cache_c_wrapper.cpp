#include "cache_c_wrapper.h"
#include "cache.h"
#include <new>

void *Cache_Allocate(long max_entries) {
  return new (std::nothrow) Solarwinds::Cache(max_entries);
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
    auto key = std::string(collector) + std::to_string(std::hash<std::string>{}(std::string(token))) + std::string(serviceName);
    c->Put(key, std::string(settings));
  }
}

zend_string *Cache_Get(void *cache, char *collector, char *token,
                       char *serviceName) {
  if (cache != nullptr && collector != nullptr && token != nullptr &&
      serviceName != nullptr) {
    auto c = static_cast<Solarwinds::Cache *>(cache);
    auto key = std::string(collector) + std::to_string(std::hash<std::string>{}(std::string(token))) + std::string(serviceName);
    auto result = c->Get(key);
    if (result.first) {
      return zend_string_init(result.second.c_str(), result.second.size(),
                              false);
    }
  }
  return nullptr;
}

void Cache_PutBucketState(void *cache, char *pid, char *bucketTokenState) {
  if (cache != nullptr && pid != nullptr && bucketTokenState != nullptr) {
    auto c = static_cast<Solarwinds::Cache *>(cache);
    c->Put(std::string(pid), std::string(bucketTokenState));
  }
}

zend_string *Cache_GetBucketState(void *cache, char *pid) {
  if (cache != nullptr && pid != nullptr) {
    auto c = static_cast<Solarwinds::Cache *>(cache);
    auto result = c->Get(std::string(pid));
    if (result.first) {
      return zend_string_init(result.second.c_str(), result.second.size(), false);
    }
  }
  return nullptr;
}
