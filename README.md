## Caching algorithms

### FIFO-cache (First-In-First-Out)

Pretty simple and naive algorithm. Every key-value pair is added in front of
doubly linked list. When list is full last node is evicted. So this algorithm 
does not consider both frequency and recency. To make all operation approximately
O(1) we need also hash map storing pointers on elements in list.

### LRU-cache (Least Recently Used)

This cache is an upgrade of first one. Here we use recency. New element is added
in front of list as well as in FIFO-cache but operation `get` do one extra thing:
it transfer accessing element to front. So list is always sorted by recency.

This implementation is little bit tested for my own and not template (int version)
was successfully submitted on leetcode 
([Problem 146](https://leetcode.com/problems/lru-cache/)). For this reason the 
implementation is pretty reliable.

### LFU-cache (Least Frequently Used)

This cache is an upgrade of LRU. Here we look at frequency of elements when decide
what to evict. This implementation is based on hash map of frequency -> list of elements
and hash map of key -> element. Here might be some optimization in the future.

Code is tested on leetcode ([Problem 460](https://leetcode.com/problems/lfu-cache/)).

### Benchmarking

Data for benchmarking was took on [umass](https://traces.cs.umass.edu/docs/traces/) and
[wiki](https://dumps.wikimedia.org/other/pagecounts-raw/).
