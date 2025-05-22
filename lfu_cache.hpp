#pragma once

#include <list>
#include <unordered_map>
#include <iostream>

#include "base_cache.hpp"


namespace cache {

template <typename T, typename KeyT = int>
class lfu_cache: public cache<T, KeyT> {
public:
    lfu_cache(const std::size_t capacity): cache<T, KeyT>(capacity), minFreq_(0) {}
    
    bool full() const { return cache_.size() >= this->capacity_;}

    std::optional<T> get(const KeyT& key) {
        auto hit = hash_.find(key);
        if (hit == hash_.end()) return std::nullopt;
        T result = std::get<2>(*hit->second);
        updateFrequency_(hit->second);
        return result;
    }

    void put(const KeyT& key, const T& value) {
        auto hit = hash_.find(key);
        if (hit != hash_.end()) {
            std::get<2>(*hit->second) = value;
            updateFrequency_(hit->second);
        } else {
           if (full()) {
                auto [head, tail] = freqMap_[minFreq_];
                hash_.erase(std::get<0>(*tail));
                freqMap_[minFreq_].second = std::prev(tail);
                cache_.erase(tail); 
            } 
            minFreq_ = 1;
            add_(key, minFreq_, value);
        }
    }

private:
    using Node = typename std::tuple<KeyT, std::size_t, T>;
    using ListIt = typename std::list<Node>::iterator;

    void add_(KeyT key, std::size_t freq, T value){
        if (freqMap_.find(freq) == freqMap_.end()) {
            cache_.emplace_front(key, freq, value);
            freqMap_[freq] = {cache_.begin(), cache_.begin()};
            hash_.emplace(key, cache_.begin());
        } else {
            cache_.emplace(freqMap_[freq].first, key, freq, value);
            hash_.emplace(key, freqMap_[freq].first);
            auto [head, tail] = freqMap_[freq];
            if (head == tail) freqMap_[freq].first = std::prev(head);
        }
    } 

    void updateFrequency_(ListIt it)  {
        std::size_t oldFreq = std::get<1>(*it)++;
        auto [head, tail] = freqMap_[oldFreq];
        if (head == tail) { 
            freqMap_.erase(oldFreq);
            if (minFreq_ == oldFreq) minFreq_++;
        }
        auto [key, freq, value] = *it;
        cache_.erase(it);
        add_(key, freq, value);
    }


    std::size_t minFreq_;
    std::list<Node> cache_;
    std::unordered_map<KeyT, ListIt> hash_;
    // maps frequency to a pair of head and tail pointers
    std::unordered_map<KeyT, std::pair<ListIt, ListIt>> freqMap_;
};

} // namespace cache

