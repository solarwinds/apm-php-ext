#include "cache.h"

namespace Solarwinds {
void Cache::Put(const std::string &collector, const std::string &token,
                const std::string &serviceName, const std::string &settings) {
  std::lock_guard<std::mutex> lock(cache_mutex_);
  cache_[std::make_tuple(collector, std::hash<std::string>{}(token),
                         serviceName)] = settings;
}
std::pair<bool, std::string> Cache::Get(const std::string &collector,
                                        const std::string &token,
                                        const std::string &serviceName) {
  auto key =
      std::make_tuple(collector, std::hash<std::string>{}(token), serviceName);
  {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end()) {
      return std::make_pair(true, it->second);
    }
  }
  return std::make_pair(false, "");
}
} // namespace Solarwinds

/*
#include <list>
#include <unordered_map>
#include <utility>

template<typename Key, typename Value>
class LRUCache {
public:
  LRUCache(size_t capacity) : capacity_(capacity) {}

  bool get(const Key& key, Value& value) {
    auto it = cache_map_.find(key);
    if (it == cache_map_.end())
      return false;
    // Move the accessed item to the front of the list
    cache_items_.splice(cache_items_.begin(), cache_items_, it->second);
    value = it->second->second;
    return true;
  }

  void put(const Key& key, const Value& value) {
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
      // Update value and move to front
      it->second->second = value;
      cache_items_.splice(cache_items_.begin(), cache_items_, it->second);
    } else {
      if (cache_items_.size() == capacity_) {
        // Remove least recently used
        auto last = cache_items_.end();
        --last;
        cache_map_.erase(last->first);
        cache_items_.pop_back();
      }
      cache_items_.emplace_front(key, value);
      cache_map_[key] = cache_items_.begin();
    }
  }

private:
  size_t capacity_;
  std::list<std::pair<Key, Value>> cache_items_;
  std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator>
cache_map_;
};
*/