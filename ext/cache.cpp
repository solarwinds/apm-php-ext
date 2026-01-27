#include "cache.h"

namespace Solarwinds {
    void Cache::Put(const std::string& collector, const std::string& token, const std::string& serviceName, const std::string& settings) {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        cache_[std::make_tuple(collector, token, serviceName)] = settings;
    }
    std::optional<std::string> Cache::Get(const std::string& collector, const std::string& token, const std::string& serviceName) {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        auto key = std::make_tuple(collector, token, serviceName);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
}
