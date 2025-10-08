# HashMap (C++)

A custom implementation of a hash map.  
The data structure is based on an array of buckets and **separate chaining** (linked lists).  
Provides average-case **O(1)** access, insertion, and deletion, with possible **O(n)** degradation in case of collisions.

---

## Features
- Generic key–value storage (`template <typename K, typename V>`).  
- Separate chaining with linked lists for collision handling.  
- Custom memory management (manual allocation, destruction, resizing).  
- Dynamic resizing when load factor threshold is exceeded.  
- Supports insertion, lookup, deletion, clearing, and reset.  
- Optional return type via `std::optional<V>` for safe lookups.
- Bitwise index calculation (`index = hash & (capacity - 1)`), requiring capacity to be a power of two.  

---

## Complexity
| Operation | Average case | Worst case (collisions) |
|-----------|--------------|--------------------------|
| `get`     | O(1)         | O(n) |
| `put`     | O(1)         | O(n) |
| `remove`  | O(1)         | O(n) |
| `resize`  | O(n)         | O(n) |

---

## Public API

```cpp
HashMap();                  // default constructor
~HashMap();                 // destructor

std::optional<V> get(const K& key) const;
void put(const K& key, const V& value);
bool remove(const K& key);

void clear();               // remove all elements, keep capacity
void reset();               // reset to default state

size_t size() const;
bool empty() const;
