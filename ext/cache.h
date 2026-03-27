#ifndef APM_PHP_EXT_CACHE_H
#define APM_PHP_EXT_CACHE_H

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

/**
 * Simple in-memory cache for storing value based on a key.
 */
namespace Solarwinds {

class Cache {
public:
  /**
   * Constructor to initialize the cache with a maximum number of entries.
   * @param max_entries The maximum number of entries the cache can hold.
   */
  Cache(size_t max_entries);
  /**
   * Store settings in the cache.
   *
   * @param key The key.
   * @param value The value.
   */
  void Put(const std::string &key, const std::string &value);
  /**
   * Retrieve settings from the cache.
   *
   * @param key The key.
   * @return A pair where the first element indicates if the value were
   * found, and the second element is the value (if found).
   */
  std::pair<bool, std::string> Get(const std::string &key);

private:
  /**
   * Maximum number of entries the cache can hold.
   */
  size_t max_entries_;
  /**
   * The underlying cache storage as a list to maintain LRU order.
   */
  std::list<std::pair<std::string, std::string>> cache_;
  /**
   * Map for quick lookup of cache entries.
   */
  std::unordered_map<std::string,
                     std::list<std::pair<std::string, std::string>>::iterator>
      cache_map_;
  /**
   * Mutex to ensure thread-safe access to the cache.
   */
  std::mutex cache_mutex_;
};
} // namespace Solarwinds
#endif // APM_PHP_EXT_CACHE_H
