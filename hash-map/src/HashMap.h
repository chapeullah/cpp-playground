#ifndef CPPHASHMAP_HASHMAP_H
#define CPPHASHMAP_HASHMAP_H

#include <functional> // std::hash
#include <optional>

/**
 * @file HashMap.h
 * @brief Реализация собственной хеш-таблицы.
 *
 * Структура данных на основе массива бакетов и цепочек (separate chaining).
 * В среднем доступ к элементам выполняется за O(1), но при коллизиях операции
 * могут деградировать до O(n) в рамках одного бакета.
 *
 * Для вычисления индекса используется побитовое И:
 *     index = hash & (capacity - 1)
 *
 * Такая схема корректна только при условии, что capacity является степенью двойки.
 */


template<typename K, typename V>
struct Node
{
    /// Ключ элемента
    const K key;

    /// Значение элемента
    V value;

    /// Хеш элемента, хранится для оптимизации, чтобы каждый раз не считать его
    const size_t hash;

    /// Указатель не следующий элемент
    Node *next;

    Node(const K &k, const V &v, const size_t h, Node *n = nullptr)
        : key(k), value(v), hash(h), next(n) {}
};

template <typename K, typename V>
class HashMap
{
    /// Массив указателей га бакеты
    Node<K, V> **buckets = nullptr;

    /// Текущее количество элементов
    size_t sz = 0;

    /// Размер массива бакетов
    size_t capacity = 16;

    /// Коэффициент загрузки
    float load_factor = 0.75f;

    /// Порог для resize
    size_t threshold = static_cast<size_t>(capacity * load_factor);

    /// Хеш-функция
    std::hash<K> hasher;

    /**
     * @brief Инициализирует хеш-таблицу.
     *
     * Создает массив бакетов длиной @p cap и сбрасывает текущий размер (size)
     * в 0.
     * Рассчитывается новое значение threshold на основе load_factor и capacity.
     *
     * @param cap количество бакетов (по умолчанию 16).
     *            Должно быть степенью двойки, иначе хеширование индекса
     *            работать корректно не будет.
     *
     * @note Вызывается в конструкторе и при изменении размера таблицы (resize).
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
     * @brief Увеличение размера массива бакетов вдвое.
     *
     * Создает новый массив бакетов вдвое большего размера, выполняет
     * init(cap * 2) и распределяет все элементы из старого массива в новый.
     *
     * @note Средняя сложность O(n), где n - количество элементов.
     *       Вызывается автоматически при превышении threshold в методе put().
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
    HashMap& operator=(const HashMap&) = delete; // надо понять TODO

    ~HashMap()
    {
        clear();
        delete[] buckets;
    }

    /**
     * @brief Возвращает значение по ключу.
     *
     * @param key ключ
     * @return std::optional<V> - если найдено значение, иначе std::nullopt
     *
     * @note Средняя сложность O(1), но при большом количестве коллизий может
     * быть O(n) для одного бакета
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
     * @brief Добавление или обновление пары "ключ-значение" в хеш-таблице.
     *
     * Если ключ уже существует значение перезаписывается, в противном случае
     * создается новый элемент
     *
     * @param key ключ
     * @param value значение
     *
     * @note Средняя сложность O(1). Может вызвать resize() при превышении
     * threshold
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
     * @brief Удаление элемента по ключу.
     *
     * @param key ключ
     * @return true - элемент был найден и успешно удалён;
     *         false - элемент с таким ключом отсутствует
     *
     * @note Средняя сложность O(1), но при большом количестве коллизий может
     *       быть O(n) для одного бакета
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
     * @brief Очистка хеш-таблицы.
     *
     * Удаляет все элементы, но оставляет текущее значение capacity и threshold.
     * После вызова таблица будет пустой, однако выделенная память под бакеты
     * останется прежнего размера.
     *
     * @note Средняя сложность O(n), где n - количество элементов
     * @warning После вызова все указатели на элементы становятся
     *          недействительными.
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
     * @brief Полный сброс хеш-таблицы.
     *
     * Удаляет все элементы и освобождает массив бакетов, после чего
     * инициализирует таблицу заново со стандартными значениями.
     * Эквивалентно вызову конструктора по умолчанию.
     *
     * @note Используется, если нужно освободить память и вернуть таблицу в
     *       исходное состояние.
     * @warning После вызова все указатели на элементы становятся
     *          недействительными.
     */
    void reset() // очистка и полный сброс к начальным значениям
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

    [[nodiscard]] size_t size() const // размер
    {
        return sz;
    }

    [[nodiscard]] bool empty() const // пустота
    {
        return sz == 0;
    }

};

#endif //CPPHASHMAP_HASHMAP_H