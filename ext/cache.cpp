#include "cache.h"

namespace Solarwinds {
Cache::Cache(size_t capacity) : capacity_(capacity) {}

void Cache::Put(const std::string &collector, const std::string &token,
                const std::string &serviceName, const std::string &settings) {
  auto key =
      collector + std::to_string(std::hash<std::string>{}(token)) + serviceName;
  std::lock_guard<std::mutex> lock(cache_mutex_);
  if (cache_map_.find(key) == cache_map_.end()) {
    // new
    while (cache_.size() > capacity_ - 1) {
      auto it = cache_.front();
      cache_map_.erase(it.first);
      cache_.pop_front();
    }
    cache_.push_back(std::make_pair(key, settings));
    cache_map_[key] = std::prev(cache_.end());
  } else {
    // update, move cache
    auto it = cache_map_[key];
    cache_.erase(it);
    cache_.push_back(std::make_pair(key, settings));
    cache_map_[key] = std::prev(cache_.end());
  }
}
std::pair<bool, std::string> Cache::Get(const std::string &collector,
                                        const std::string &token,
                                        const std::string &serviceName) {
  auto key =
      collector + std::to_string(std::hash<std::string>{}(token)) + serviceName;
  std::lock_guard<std::mutex> lock(cache_mutex_);
  auto it = cache_map_.find(key);
  if (it == cache_map_.end()) {
    return std::make_pair(false, "");
  }
  auto cache_iterator = it->second;
  auto settings = cache_iterator->second;
  cache_.erase(cache_iterator);
  cache_.push_back(std::make_pair(key, settings));
  cache_map_[key] = std::prev(cache_.end());
  return std::make_pair(true, settings);
}
} // namespace Solarwinds
