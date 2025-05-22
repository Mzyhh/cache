#pragma once

#include "base_cache.hpp"
#include "fifo_cache.hpp"
#include "lru_cache.hpp"


namespace cache {

template<typename T, typename KeyT=int>
class q2_cache : public cache<KeyT, T> {
public:
    q2_cache(const int capacity, const double hot_part=0.2, const double in_part=0.2):
        cache<KeyT, T>(capacity), hot(capacity * hot_part), in(capacity * in_part),
        out(capacity - int(capacity * hot_part) - int(capacity * in_part)){
    }

    bool full() const {
        return in.full() && out.full() && hot.full();
    }

    std::optional<T> get(const KeyT& key) {
        auto v = hot.get(key); 
        if (v) return v;

        v = in.get(key);
        if (v) return v;

        v = out.get(key);
        if (v) {
            hot.put(key, v.value());
            return v;
        }

        return std::nullopt;
    }

    void put(const KeyT& key, const T& value) {
        if (hot.get(key)) {
            hot.put(key, value); 
            return;
        }
        if (in.get(key)) {
            in.put(key, value);
            return;
        }
        if (out.get(key)) {
            hot.put(key, value);
            return;
        }

        if (in.full()) {
            auto v = in.pop();
            if (v) out.put(key, *v);
            in.put(key, value);
        }
    }
private:
    fifo_cache<T, KeyT> in, out;
    lru_cache<T, KeyT> hot;
};

} // namespace cache
