#ifndef APM_PHP_EXT_CACHE_H
#define APM_PHP_EXT_CACHE_H

#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>

/**
 * Simple in-memory cache for storing settings based on collector, token, and service name.
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
        void Put(const std::string& collector, const std::string& token, const std::string& serviceName, const std::string& settings);
        /**
         * Retrieve settings from the cache.
         *
         * @param collector The collector endpoint.
         * @param token The token.
         * @param serviceName The name of the service.
         * @return An optional string containing the settings if found, or std::nullopt if not found.
         */
        std::optional<std::string> Get(const std::string& collector, const std::string& token, const std::string& serviceName);
    private:
        /**
         * The in-memory cache storing settings.
         * The key is a tuple of (collector, token, serviceName).
         * The value is the cached settings string.
         */
        std::map<std::tuple<std::string, std::string, std::string>, std::string> cache_;
        /**
         * Mutex to ensure thread-safe access to the cache.
         */
        std::mutex cache_mutex_;
    };
}
#endif //APM_PHP_EXT_CACHE_H
