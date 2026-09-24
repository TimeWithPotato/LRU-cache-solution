# LRU Cache (C++)

This is my solution for Task 2 - LRU Cache. It is a Least Recently Used (LRU) cache written in C++
that supports `get(key)` and `put(key, value)` in O(1) average time.
I also implemented the optional TTL (expiration) bonus.

## Files

- `lru_cache.cpp` - source code of the `LRUCache` class and the test/example code in `main()`
- `output.png` - screenshot of the program output (put, get, LRU eviction, returned values and TTL)
- `README.md` - this file

## How to run

You need a C++ compiler (g++) with C++11 or newer.

```bash
g++ lru_cache.cpp -o lru_cache
./lru_cache
```

On Windows PowerShell use `./lru_cache` (or `lru_cache.exe`).

## How to use the class

```cpp
LRUCache cache(2);          // capacity must be positive
cache.put("A", 10);         // insert or update
cache.put("C", 30, 2000);   // optional ttl in milliseconds
cache.get("A");             // returns the value, or -1 if not found / expired
```

If the capacity is 0 or negative, the constructor throws an `invalid_argument` exception.

## Data structures used and why

1. **Hash map (`unordered_map<string, Node*>`)**
   It stores key -> pointer to the node, so I can find any key in O(1) on average.

2. **Doubly linked list**
   It keeps the keys in order of use. The node right after `head` is the most recently used one
   and the node right before `tail` is the least recently used one.
   I used a doubly linked list because I can remove a node from the middle in O(1) when I
   already have its pointer (from the map). With a singly linked list I would need to search for
   the previous node, which is O(n).
   I also used two dummy nodes (`head` and `tail`) so I don't need special checks for an empty list
   or for the first/last element.

## How LRU ordering is maintained

- `get(key)`: if the key is not in the map, return -1. Otherwise move its node to the front
  (most recently used) and return the value.
- `put(key, value)` when the key already exists: update the value and move the node to the front.
- `put(key, value)` when the key is new: if the cache is full, remove the node before `tail`
  (least recently used) from the list and from the map. Then add the new node at the front
  and store it in the map.

So the most recently used key is always at the front and the least recently used key is always
at the back, which is the one removed when the capacity is exceeded.

## Time complexity

- `get`: O(1) average
- `put`: O(1) average

The hash map gives O(1) average lookup, and adding/removing a node in the doubly linked list is O(1).

## Space complexity

O(capacity), because the cache never stores more than `capacity` keys
(one map entry and one linked list node for each key).

## Bonus: TTL / expiration

`put(key, value, ttl)` accepts an optional time to live in milliseconds. If `ttl` is 0 (default),
the key never expires.

**Approach:** every node has an `expiry` field which stores (current time + ttl).
When `get(key)` is called, I check whether the current time is greater than `expiry`.
If yes, I remove the node from the list and the map and return -1.
If a key is updated with `put()`, its expiry is also reset.
I used `steady_clock` for the time so it does not depend on the system clock changing.

**Trade-offs:**
- This is *lazy* expiration, so an expired key is only removed when `get()` is called on it.
  Until that happens it stays in memory and still counts towards the capacity.
  Removing expired keys automatically (background thread, or a min-heap of expiry times) would free
  memory earlier, but it would make the code more complex.
- The cache is not thread-safe because I did not use any locks.

## Test cases in `main()`

1. **Test 1** - the example from the task (capacity 2): `put(A)`, `put(B)`, `get(A)`, `put(C)` evicts `B`,
   then `get(B)` = -1, `get(C)` = 30, `get(A)` = 10.
2. **Test 2** - updating an existing key: updating `X` makes it most recently used, so `Y` is evicted when `Z` is added.
3. **Test 3 (bonus)** - TTL: a key with a 1000 ms TTL is returned before it expires and gives -1 after waiting 1500 ms.

## Sample output

```
Test 1 - example from the task
put(A, 10)
put(B, 20)
get(A) = 10
put(C, 30)
   (cache full, evicting LRU key "B")
get(B) = -1
get(C) = 30
get(A) = 10

Test 2 - updating a key
put(X, 1)
put(Y, 2)
put(X, 100)
put(Z, 3)
   (cache full, evicting LRU key "Y")
get(Y) = -1
get(X) = 100
get(Z) = 3

Test 3 - TTL (bonus)
put(T, 99) with ttl 1000 ms
get(T) = 99
waiting 1500 ms...
get(T) = -1
```

The same output is shown in `output.png`.
