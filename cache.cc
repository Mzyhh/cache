#include <cassert>
#include <iostream>

#include "lru_cache.hpp"
#include "lfu_cache.hpp"
#include "fifo_cache.hpp"

// slow get page imitation
int slow_get_page_int(int key) { return key; }

int main(int argc, char *argv[]) {
    int hits{0};
    int nelems{0};
    size_t cache_capacity{0};
    std::cin >> cache_capacity >> nelems;
    assert(std::cin.good());
    cache::lfu_cache<int> c(cache_capacity);
    for (int i = 0; i < nelems; ++i) {
        int key{0};
        std::cin >> key;
        assert(std::cin.good());
        if (cache::lookup_update(c, key, slow_get_page_int)) hits++;
//        if (c.get(key) != std::nullopt) {
//            std::cout << "Found element" << std::endl;
//        } else {
//            c.put(key, slow_get_page_int(key));
//            std::cout << "Put element" << std::endl;
//        }
    }
    std::cout << c.cache_.size() << ' ' << c.hash_.size() << ' ' << c.freqMap_.size() << std::endl;
    
    std::cout << hits << std::endl;
}
