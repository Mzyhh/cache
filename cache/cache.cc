#include <cassert>
#include <iostream>

#include "cache.hpp"

// slow get page imitation
int slow_get_page_int(int key) { return key; }

int main(int argc, char *argv[]) {
    int hits{0};
    int nelems{0};
    size_t cache_capacity{0};
    std::cin >> cache_capacity >> nelems;
    assert(std::cin.good());
    caches::cache_t<int> c(cache_capacity);
    for (int i = 0; i < nelems; ++i) {
        int key{0};
        std::cin >> key;
        assert(std::cin.good());
        if (c.lookup_update(key, slow_get_page_int)) hits++;
    }
    
    std::cout << hits / static_cast<double>(nelems) << std::endl;
}
