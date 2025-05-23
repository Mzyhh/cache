#pragma once

#include <list>
#include <unordered_map>

#include "base_cache.hpp"


namespace caches {

template <typename T, typename KeyT = int>
class lfu_cache: public cache<T, KeyT> { 
public:
    lfu_cache(const std::size_t capacity): cache<T, KeyT>(capacity), minFreq_(1) {}
    
    bool full() const { return cache_.size() >= this->capacity_;}

    std::optional<T> get(const KeyT& key) {
        auto hit = cache_.find(key);
        if (hit != cache_.end()) {
            T value = std::get<2>(*hit->second);
            update_(hit->second);
            return value;
        }
        return std::nullopt;
    }

    void put(const KeyT& key, const T& value) {
        auto hit = cache_.find(key);
        if (hit != cache_.end()) {
            std::get<2>(*hit->second) = value;
            update_(hit->second);
        } else {
            if (full()) {
                remove_last_();
            }
            add_new_(key, value);
        }
    }

private:
    using Node = std::tuple<KeyT, std::size_t, T>;

    int add_new_(const KeyT& key, const T& value) {
        minFreq_ = 1;
        if (freqMap_.find(minFreq_) == freqMap_.end())
            freqMap_.emplace(minFreq_, std::list<Node>());
        freqMap_[minFreq_].emplace_front(key, minFreq_, value);
        cache_[key] = freqMap_[minFreq_].begin();
        return 0;
    }

    int remove_last_() {
        if (freqMap_.empty()) return -1;
        auto& [key, freq, value] = freqMap_[minFreq_].back();
        cache_.erase(key);
        freqMap_[minFreq_].pop_back();
        if (freqMap_[minFreq_].empty()) {
            freqMap_.erase(minFreq_);
        }
        return 0;
    }
   
    // `it` refers to element in `cache_`
    int update_(typename std::list<Node>::iterator it) {
        auto [key, freq, value] = *it;
        freqMap_[freq].erase(it); 
        if (freqMap_[freq].empty()) {
            if (minFreq_ == freq) minFreq_++;
            freqMap_.erase(freq);
        }
        if (freqMap_.find(freq + 1) == freqMap_.end()) {
            freqMap_.emplace(freq + 1, std::list<Node>());
        }
        freqMap_[freq + 1].emplace_front(key, freq + 1, value);
        cache_[key] = freqMap_[freq + 1].begin();
        return 0;
    }

    std::size_t minFreq_;
    std::unordered_map<KeyT, typename std::list<Node>::iterator> cache_;
    std::unordered_map<std::size_t, std::list<Node>> freqMap_;
};

} // namespace cache

