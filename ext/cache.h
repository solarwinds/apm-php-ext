#ifndef APM_PHP_EXT_CACHE_H
#define APM_PHP_EXT_CACHE_H

#include <mutex>
#include <string>
#include <tuple>
#include <map>

/**
 * Simple in-memory cache for storing settings based on collector, token, and
 * service name.
 */
namespace Solarwinds {

/**
 * Hash function for tuples to be used in unordered_map.
 */
  /*
struct TupleHash {
  template <class T> struct component {
    const T &value;
    component(const T &value) : value(value) {}
    size_t operator,(size_t n) const {
      // Combine hash of current component with the accumulated hash, 0x9e3779b9
      // is the integral part of the Golden Ratio's fractional part
      // 0.61803398875…
      n ^= std::hash<T>{}(value) + 0x9e3779b9 + (n << 6) +
           (n >> 2); // Hash combine formula
      return n;
    }
  };

  template <class Tuple> size_t operator()(const Tuple &tuple) const {
    size_t seed = 0;
    std::apply([&](const auto &...xs) { (component(xs), ..., seed); }, tuple);
    return seed;
  }
};
*/

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
  /**
   * The in-memory cache storing settings.
   * The key is a tuple of (collector, std::hash<std::string>{}(token),
   * serviceName). The value is the cached settings string.
   */
  std::map<std::tuple<std::string, std::size_t, std::string>,
                     std::string>
      cache_;
  /**
   * Mutex to ensure thread-safe access to the cache.
   */
  std::mutex cache_mutex_;
};
} // namespace Solarwinds
#endif // APM_PHP_EXT_CACHE_H
