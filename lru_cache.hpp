#pragma once

#include <list>
#include <unordered_map>

#include "base_cache.hpp"


namespace caches {

template <typename KeyT, typename T>
class lru_cache: public cache<KeyT, T>{
public:
    lru_cache(const std::size_t size): cache<KeyT, T>(size) {}

    bool full() const { return cache_.size() == this->capacity_; }
    
    std::optional<T> get(const KeyT& key) {
        auto hit = hash_.find(key);
        if (hit == hash_.end()) return std::nullopt;
        cache_.splice(cache_.begin(), cache_, hit->second);
        return hit->second->second; 
    }

    void put(const KeyT& key, const T& value) {
        auto hit = hash_.find(key);
        if (hit != hash_.end()) {
            hit->second->second = value; 
            cache_.splice(cache_.begin(), cache_, hit->second);
        } else {
            if (full()) {
                hash_.erase(cache_.back().first);
                cache_.pop_back();
            }
            cache_.emplace_front(key, value);
            hash_.emplace(key, cache_.begin());
        }
    }

    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page) {
        auto hit = hash_.find(key); 
        if (hit == hash_.end()) {
            if (full()) {
                hash_.erase(cache_.back().first);
                cache_.pop_back();
            }
            cache_.emplace_front(key, slow_get_page(key));
            hash_.emplace(key, cache_.begin());
            return false;
        }
        auto eltit = hit->second;
        cache_.splice(cache_.begin(), cache_, eltit);
        return true;
    }
private:
    std::list<std::pair<KeyT, T>> cache_;
    using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
    std::unordered_map<KeyT, ListIt> hash_;
};

} // namespace cache

