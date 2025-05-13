#pragma once

#include <list>
#include <unordered_map>

#include "base_cache.hpp"


namespace cache {

template <typename T, typename KeyT = int>
class lfu_cache: public cache<T, KeyT> {
public:
    lfu_cache(const std::size_t capacity): cap_(capacity), minFreq_(0) {}
    
    bool full() const { return cache_.size() == cap_;}

    std::optional<T> get(const KeyT& key) {
        auto hit = hash_.find(key);
        if (hit == hash_.end()) return std::nullopt;
        T result = std::get<2>(hit->second);
        updateFrequency_(hit->second);
        return result;
    }

    void put(const KeyT& key, const T& value) {
        auto hit = hash_.find(key);
        if (hit != hash_.end()) {
            std::get<2>(hit->second) = value;
            updateFrequency_(hit->second);
        } else {
           if (full()) {
                auto [head, tail] = freqMap_[minFreq_];
                freqMap_[minFreq_]->second--;
                hash_.erase(std::get<0>(*tail));
                cache_.erase(tail); 
            } 
            minFreq_ = 1;
            add_(key, minFreq_, value);
        }
    }

    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page) {
        auto hit = hash_.find(key);
        if (hit == hash_.end()) {
            if (full()) {
                auto [head, tail] = freqMap_[minFreq_];
                hash_.erase(std::get<0>(*tail));
                cache_.erase(tail);
                if (head == tail) freqMap_.erase(minFreq_); 
            } 
            minFreq_ = 1; // reset minimum frequency
            add_(key, minFreq_, slow_get_page(key));
            return false;
        }

        return true;
    }
private:
    using Node = typename std::tuple<KeyT, std::size_t, T>;
    using ListIt = typename std::list<Node>::iterator;

    void add_(KeyT key, std::size_t freq, T value) {
        if (freqMap_.find(freq) == freqMap_.end()) {
            cache_.emplace_front(key, freq, value);
            freqMap_[freq] = {cache_.begin(), cache_.begin()};
            hash_.emplace(key, cache_.begin());
        } else {
            cache_.emplace(freqMap_[freq].first, key, freq, value);
            hash_.emplace(key, freqMap_[freq].first);
        }
    } 

    void updateFrequency_(ListIt it) {
        std::size_t oldFreq = std::get<1>(it->second)++;
        auto [head, tail] = freqMap_[oldFreq];
        if (head == tail) { 
            freqMap_.erase(oldFreq);
            if (minFreq_ == oldFreq) minFreq_++;
        }
        auto [key, freq, value] = *it;
        add_(key, freq, value);
    }


    std::size_t cap_;
    std::size_t minFreq_;
    std::list<Node> cache_;
    std::unordered_map<KeyT, ListIt> hash_;
    // maps frequency to a pair of head and tail pointers
    std::unordered_map<KeyT, std::pair<ListIt, ListIt>> freqMap_;
};

} // namespace cache

