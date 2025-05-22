#pragma once

#include <list>
#include <unordered_map>

#include "base_cache.hpp"


namespace cache {

template <typename T, typename KeyT = int>
class lfu_cache: public cache<T, KeyT> {
public:
    lfu_cache(const std::size_t capacity): cache<T, KeyT>(capacity), minFreq_(1) {}
    
    bool full() const { return cache_.size() >= this->capacity_;}

    std::optional<T> get(const KeyT& key) {
        auto it = hash_.find(key);
        if (it == hash_.end())
            return std::nullopt;
        updateFrequency_(key); 
        return it->second->value;
    }

    void put(const KeyT& key, const T& value) {
        auto it = hash_.find(key);
        if (it != hash_.end()) {
            it->second->value = value;  
            updateFrequency_(key);
        } else {
            if (full()) {
                remove_freqMap_tail_(minFreq_);
            }
            minFreq_ = 1;
            add_(key, minFreq_, value);
        }
    }

private:
    struct Node {
        KeyT key;
        std::size_t freq;
        T value;

        Node(const KeyT& key, const std::size_t freq, const T& value): key(key), freq(freq), value(value) {}
    };
    using ListIt = typename std::list<Node>::iterator;

    void add_(KeyT key, std::size_t freq, T value){
        if (freqMap_.find(freq) != freqMap_.end()) {
            auto head = freqMap_[freq].first;
            auto new_node = cache_.emplace(head, key, freq, value);
            freqMap_[freq].first = new_node; 
            hash_.emplace(key, new_node);
        } else {
            cache_.emplace_front(key, freq, value);  
            hash_.emplace(key, cache_.begin());
            freqMap_[freq] = {cache_.begin(), cache_.begin()};
        }
    }

    void remove_(const KeyT& key) {
        auto it = hash_[key];
        if (it == freqMap_[it->freq].second) { 
            remove_freqMap_tail_(it->freq);
            return;
        }
        if (it == freqMap_[it->freq].first) {
            freqMap_[it->freq].first = std::next(freqMap_[it->freq].first); 
        }
        cache_.erase(it);
        hash_.erase(it->key);
    }

    void remove_freqMap_tail_(std::size_t freq) {
        auto& [head, tail] = freqMap_[freq];
        if (head == tail) { 
            freqMap_.erase(freq);
            update_minfreq_();
        }
        else freqMap_[freq].second = std::prev(freqMap_[freq].second);
        hash_.erase(tail->key);
        cache_.erase(tail);
    }

    void update_minfreq_() {
        while (freqMap_.find(minFreq_) == freqMap_.end()) minFreq_++; 
    }

    void updateFrequency_(const KeyT& key)  {
        auto [k, freq, value] = *hash_[key];
        remove_(key);
        add_(k, freq + 1, value); 
    }

    std::size_t minFreq_;
    std::list<Node> cache_;
    std::unordered_map<KeyT, ListIt> hash_;
    // maps frequency to a pair of head and tail pointers
    std::unordered_map<std::size_t, std::pair<ListIt, ListIt>> freqMap_;
};

} // namespace cache

