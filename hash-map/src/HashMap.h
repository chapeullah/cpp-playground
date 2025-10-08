#ifndef CPPHASHMAP_HASHMAP_H
#define CPPHASHMAP_HASHMAP_H

#include <functional> // std::hash
#include <optional>

/**
 * @file HashMap.h
 * @brief Implementation of a custom hash map.
 *
 * Data structure based on an array of buckets and separate chaining (linked
 * list). Average access time is O(1), but in case of collisions operations may
 * degrade to O(n) within a single bucket.
 *
 * Index is calculated using bitwise AND:
 *     index = hash & (capacity - 1)
 *
 * This scheme works correctly only if capacity is a power of two.
 */

template<typename K, typename V>
struct Node
{
    /// Key of the element
    const K key;

    /// Value of the element
    V value;

    /// Hash of the element, stored to avoid recalculating
    const size_t hash;

    /// Pointer to the next element in the chain
    Node *next;

    Node(const K &k, const V &v, const size_t h, Node *n = nullptr)
        : key(k), value(v), hash(h), next(n) {}
};

template <typename K, typename V>
class HashMap
{
    /// Array of bucket pointers
    Node<K, V> **buckets = nullptr;

    /// Current number of elements
    size_t sz = 0;

    /// Current bucket array size
    size_t capacity = 16;

    /// Load factor
    float load_factor = 0.75f;

    /// Threshold for resize
    size_t threshold = static_cast<size_t>(capacity * load_factor);

    /// Hash function
    std::hash<K> hasher;

    /**
     * @brief Initializes the hash map.
     *
     * Creates a bucket array of length @p cap and resets the current size to 0.
     * Recalculates threshold based on load_factor and capacity.
     *
     * @param cap number of buckets (default 16).
     *            Must be a power of two, otherwise index hashing will not work
     *            correctly.
     *
     * @note Called in the constructor and when resizing the table.
     */
    void init(const size_t cap = 16)
    {
        capacity = cap;
        sz = 0;
        threshold = static_cast<size_t>(capacity * load_factor);
        buckets = new Node<K, V>*[capacity];
        for (size_t i = 0; i < capacity; ++i)
        {
            buckets[i] = nullptr;
        }
    }

    /**
     * @brief Doubles the bucket array size.
     *
     * Creates a new bucket array of twice the size, calls init(cap * 2) and
     * redistributes all elements from the old array into the new one.
     *
     * @note Average complexity is O(n), where n is the number of elements.
     *       Called automatically when threshold is exceeded in put().
     */
    void resize()
    {
        const size_t old_cap = capacity;
        const size_t temp_sz = sz;
        Node<K, V> **old_buckets = buckets;
        init(old_cap * 2);
        sz = temp_sz;

        for (size_t i = 0; i < old_cap; ++i)
        {
            Node<K, V> *e = old_buckets[i];
            while (e)
            {
                size_t index = e->hash & (capacity - 1);
                Node<K, V> *next = e->next;
                e->next = buckets[index];
                buckets[index] = e;
                e = next;
            }
            old_buckets[i] = nullptr;
        }
        delete[] old_buckets;
    }

protected:

    [[nodiscard]] size_t getCapacity() const
    {
        return capacity;
    }

    [[nodiscard]] float getLoadFactor() const
    {
        return load_factor;
    }

    [[nodiscard]] size_t getThreshold() const
    {
        return threshold;
    }

public:
    HashMap()
    {
        init();
    }

    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete; // TODO

    ~HashMap()
    {
        clear();
        delete[] buckets;
    }

    /**
     * @brief Returns the value by key.
     *
     * @param key key
     * @return std::optional<V> — value if found, otherwise std::nullopt
     *
     * @note Average complexity is O(1), but with many collisions
     *       can degrade to O(n) within one bucket.
     */
    [[nodiscard]] std::optional<V> get(const K &key) const
    {
        const size_t h = hasher(key);
        const size_t index = h & (capacity - 1);

        for (Node<K, V> *e = buckets[index]; e; e = e->next)
        {
            if (e->hash == h && e->key == key)
            {
                return std::optional<V>(e->value);
            }
        }

        return std::nullopt;
    }

    /**
     * @brief Inserts or updates a key-value pair in the hash map.
     *
     * If the key already exists, the value is updated,
     * otherwise a new element is created.
     *
     * @param key the key of the element to insert or update
     * @param value the value to associate with the key
     *
     * @note Average complexity is O(1). May call resize() when threshold
     *       is exceeded.
     */
    void put(const K &key, const V &value)
    {
        const size_t h = hasher(key);
        const size_t index = h & (capacity - 1);

        for (Node<K,V> *e = buckets[index]; e; e = e->next)
        {
            if (e->hash == h && e->key == key)
            {
                e->value = value;
                return;
            }
        }

        buckets[index] = new Node<K, V>(key, value, h, buckets[index]);
        if (++sz > threshold)
        {
            resize();
        }
    }

    /**
     * @brief Removes an element by key.
     *
     * @param key the key of the element to remove
     * @return true if the element was found and removed;
     *         false if no element with such key exists
     *
     * @note Average complexity is O(1), but with many collisions
     *       can degrade to O(n) within one bucket.
     */
    bool remove(const K &key)
    {
        const size_t h = hasher(key);
        const size_t index = h & (capacity - 1);

        Node<K, V> *prev = nullptr;

        for (Node<K, V> *e = buckets[index]; e; prev = e, e = e->next)
        {
            if (e->hash == h && e->key == key)
            {
                if (prev) prev->next = e->next;
                else buckets[index] = e->next;
                delete e;
                --sz;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Clears the hash map.
     *
     * Removes all elements but keeps current capacity and threshold.
     * After the call, the map is empty but allocated memory remains.
     *
     * @note Complexity is O(n), where n is the number of elements.
     * @warning All existing pointers to elements become invalid.
     */
    void clear()
    {
        if (!buckets || sz == 0) return;
        for (size_t i = 0; i < capacity; ++i)
        {
            Node<K, V> *curr = buckets[i];
            while (curr)
            {
                Node<K, V> *temp = curr->next;
                delete curr;
                curr = temp;
            }
            buckets[i] = nullptr;
        }
        sz = 0;
    }

    /**
     * @brief Resets the hash map completely.
     *
     * Removes all elements and frees the bucket array,
     * then reinitializes the map with default parameters.
     * Equivalent to the default constructor state.
     *
     * @note Useful when you need to free memory and restore initial state.
     * @warning All existing pointers to elements become invalid.
     */
    void reset()
    {
        if (!buckets)
        {
            init();
            return;
        }
        if (sz != 0) {
            for (size_t i = 0; i < capacity; ++i)
            {
                Node<K, V> *curr = buckets[i];
                while (curr)
                {
                    Node<K, V> *next = curr->next;
                    delete curr;
                    curr = next;
                }
            }
        }
        delete[] buckets;
        buckets = nullptr;
        init();
    }

    /// Returns number of elements
    [[nodiscard]] size_t size() const
    {
        return sz;
    }

    /// Checks whether the map is empty
    [[nodiscard]] bool empty() const
    {
        return sz == 0;
    }

};

#endif //CPPHASHMAP_HASHMAP_H
