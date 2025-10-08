# Hash map

A custom implementation of a hash map.  
The data structure is based on an array of buckets and **separate chaining** (linked lists).  
Provides average-case **O(1)** access, insertion, and deletion, with possible **O(n)** degradation in case of collisions.

## Features
- Generic key–value storage (`template <typename K, typename V>`).  
- Separate chaining with linked lists for collision handling.  
- Custom memory management (manual allocation, destruction, resizing).  
- Dynamic resizing when load factor threshold is exceeded.  
- Supports insertion, lookup, deletion, clearing, and reset.  
- Optional return type via `std::optional<V>` for safe lookups.
- Bitwise index calculation (`index = hash & (capacity - 1)`), requiring capacity to be a power of two.  

## Complexity
| Operation | Average case | Worst case (collisions) |
|-----------|--------------|--------------------------|
| `get`     | O(1)         | O(n) |
| `put`     | O(1)         | O(n) |
| `remove`  | O(1)         | O(n) |
| `resize`  | O(n)         | O(n) |

## Public API

```cpp
HashMap();
~HashMap();

std::optional<V> get(const K& key) const;
void put(const K& key, const V& value);
bool remove(const K& key);

void clear();
void reset();

size_t size() const;
bool empty() const;
```

## Method descriptions

- get(const K& key) - Returns the value by key, or std::nullopt if not found.
- put(const K& key, const V& value) → Inserts or updates a key–value pair. Resizes if threshold exceeded.
- remove(const K& key) → Removes an element by key, returns true if successful.
- clear() → Removes all elements, capacity is preserved.
- reset() → Fully resets the hash map to default state (capacity 16, size 0).
- size() → Returns the number of elements in the map.
- empty() → Returns true if the map contains no elements.

## Tests

Unit tests are implemented using **GoogleTest (gtest)**.  
They verify insertion, lookup, update, deletion, clearing, and resizing logic.  

To run the tests, simply build the project with **CMake** and execute the `test_hashmap` binary produced by the build system.

