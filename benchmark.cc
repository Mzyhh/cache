#include <iostream>
#include <fstream>
#include <string>
#include <array>

#include "base_cache.hpp"
#include "fifo_cache.hpp"
#include "lru_cache.hpp"
#include "lfu_cache.hpp"
#include "q2_cache.hpp"


namespace {
    int slow_get_page_int(std::string key) { return 0; }
}

int main(int argc, char* argv[]) {
    using namespace std;
    using namespace caches;
    if (argc < 2) return -1;

    ifstream data(argv[1]);
    int cap{100};
    cout << "Enter capacity of caches: \n";
    cin >> cap;
    const size_t ncaches = 4;
    array<pair<cache<string, int>*, size_t>, ncaches> cs {
        make_pair(new fifo_cache<string, int>(cap), 0),
        make_pair(new lru_cache<string, int>(cap), 0),
        make_pair(new lfu_cache<string, int>(cap), 0),
        make_pair(new q2_cache<string, int>(cap), 0)
    };
    string key;
    size_t cnt = 0;
    while (getline(data, key)) { 
        for (int i = 0; i < ncaches; ++i) {
            if(lookup_update(cs[i].first, key, slow_get_page_int)) cs[i].second++;
        }
        cnt++;
    }
    for (auto [c, hits] : cs)
        cout << hits/double(cnt) << " ";
    cout << endl;
    data.close();
    return 0;
}
