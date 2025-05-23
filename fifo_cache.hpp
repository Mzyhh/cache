#pragma once

#include <unordered_map>
#include <list>

#include "base_cache.hpp"


namespace caches {

template <typename T, typename KeyT = int>
class fifo_cache : public cache<T, KeyT> {
public:
    fifo_cache(const std::size_t capacity): cache<T, KeyT>(capacity) {}

    bool full() const override { return cache_.size() == this->capacity_; }

    std::optional<T> get(const KeyT& key) override {
        auto hit = hash_.find(key);
        if (hit == hash_.end()) return std::nullopt;

        return hit->second->second; 
    }

    void put(const KeyT& key, const T& value) override {
        auto hit = hash_.find(key);
        if (hit != hash_.end()) {
            hit->second->second = value;
            return;
        }

        if (full()) {
            hash_.erase(cache_.back().first);
            cache_.pop_back();
        }
        cache_.emplace_front(key, value);
        hash_.emplace(key, cache_.begin());
    }

    std::optional<std::pair<KeyT, T>> pop() {
        if (cache_.empty()) return std::nullopt;
        auto [key, res] = cache_.back();
        cache_.pop_back();
        hash_.erase(key);
        return std::pair<KeyT, T>{key, res};
    }

private:
    using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
    std::list<std::pair<KeyT, T>> cache_;
    std::unordered_map<KeyT, ListIt> hash_;
};

} // namespace cache
