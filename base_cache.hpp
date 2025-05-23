#pragma once

#include <optional>


namespace cache {
template <typename T, typename KeyT = int>
class cache {
public:
    cache(const std::size_t capacity): capacity_(capacity) {}
    virtual bool full() const = 0;
    virtual std::optional<T> get(const KeyT& key) = 0;
    virtual void put(const KeyT& key, const T& value) = 0;
    std::size_t capacity() const noexcept {
        return capacity_;
    }
protected:
    std::size_t capacity_;
};

template <typename T, typename F, typename KeyT = int>
bool lookup_update(cache<KeyT, T>& cache, const KeyT& key, F slow_get_page) {
    if (cache.get(key) != std::nullopt) return true;
    cache.put(key, slow_get_page(key));
    return false;
}

} // namespace cache
