#ifndef APM_PHP_EXT_CACHE_H
#define APM_PHP_EXT_CACHE_H

#include <mutex>
#include <string>
#include <unordered_map>

/**
 * Simple in-memory cache for storing settings based on collector, token, and
 * service name.
 */
namespace Solarwinds {

class Cache {
public:
  /**
   * Store settings in the cache.
   *
   * @param collector The collector endpoint.
   * @param token The token.
   * @param serviceName The name of the service.
   * @param settings The settings to be cached.
   */
  void Put(const std::string &collector, const std::string &token,
           const std::string &serviceName, const std::string &settings);
  /**
   * Retrieve settings from the cache.
   *
   * @param collector The collector endpoint.
   * @param token The token.
   * @param serviceName The name of the service.
   * @return A pair where the first element indicates if the settings were
   * found, and the second element is the cached settings (if found).
   */
  std::pair<bool, std::string> Get(const std::string &collector,
                                   const std::string &token,
                                   const std::string &serviceName);

private:
  std::unordered_map<std::string, std::string> cache_;
  /**
   * Mutex to ensure thread-safe access to the cache.
   */
  std::mutex cache_mutex_;
};
} // namespace Solarwinds
#endif // APM_PHP_EXT_CACHE_H
