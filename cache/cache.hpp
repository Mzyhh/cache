#include <list>
#include <unordered_map>


namespace caches {

template <typename T, typename KeyT = int>
class cache_t {
public:
    cache_t(std::size_t size): sz_(size) {}

    bool full() const { return cache_.size() == sz_;}

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
    std::size_t sz_;
    std::list<std::pair<KeyT, T>> cache_;
    using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
    std::unordered_map<KeyT, ListIt> hash_;
};

} // namespace cache

