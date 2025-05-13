## Caching algorithms

### FIFO-cache (First-In-First-Out)

Pretty simple and naive algorithm. Every key-value pair is added in front of
doubly linked list. When list is full last node is evicted. So this algorithm 
does not consider both frequency and recency. To make all operation approximately
O(1) we need also hash map storing pointers on elements in list.

### LRU-cache (Least Frequently Used)

This cache is an upgrade of first one. Here we use recency. New element is added
in front of list as well as in FIFO-cache but operation `get` do one extra thing:
it transfer accessing element to front. So list is always sorted by recency.

This implementation is little bit tested for my own and not template (int version)
was successfully submitted on leetcode 
([Problem 146](https://leetcode.com/problems/lru-cache/)). For this reason the 
implementation is pretty reliable.

