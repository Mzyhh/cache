#include <list>
#include <unordered_map>
#include <tuple>


namespace caches {

template <typename T, typename KeyT = int>
class lru_cache {
public:
    lru_cache(std::size_t size): cap_(size) {}

    bool full() const { return cache_.size() == cap_;}

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
    std::size_t cap_;
    std::list<std::pair<KeyT, T>> cache_;
    using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
    std::unordered_map<KeyT, ListIt> hash_;
};

template <typename T, typename KeyT = int>
class lfu_cache {
public:
    lfu_cache(std::size_t capacity): cap_(capacity), minFreq_(0) {}
    
    bool full() const { return cache_.size() == cap_;}

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

    using Node = typename std::tuple<KeyT, std::size_t, T>;
    using ListIt = typename std::list<Node>::iterator;

    std::size_t cap_;
    std::size_t minFreq_;
    std::list<Node> cache_;
    std::unordered_map<KeyT, ListIt> hash_;
    // maps frequency to a pair of head and tail pointers
    std::unordered_map<KeyT, std::pair<ListIt, ListIt>> freqMap_;
};

} // namespace cache

