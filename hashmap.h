#pragma once

#include <optional>
#include <utility>
#include <vector>

template<class K, class V, class H = std::hash<K>>
class hashmap {
    H m_hasher;

    struct bucket {
        // true: пустая, без элемента
        // false: лежит элемент
        bool is_free = true;

        std::pair<K, V> data;
    };

    std::vector<bucket> m_buckets;

    std::size_t m_size = 0;

    // увеличивает индекс на 1 и корректно обрабатывает цикличность корзины
    void increase_index(std::size_t &index) const {
        index++;
        if (index == m_buckets.size()) {
            index = 0;
        }
    }

    // вернет указатель, где должен находиться элемент с таким ключом
    std::size_t get_index(const K &key) const {
        return m_hasher(key) % m_buckets.size();
    }

    // вернет указатель на корзину с таким ключом или
    // указатель на первую пустую корзину, если такого ключа нет
    std::size_t find_index(const K &key) const {
        std::size_t index = get_index(key);
        for (; !m_buckets[index].is_free && m_buckets[index].data.first != key; increase_index(index)) {}
        return index;
    }

    // при необходимости расширяет корзину, чтобы поддерживать быстрый доступ к элементам
    void update_capacity() {
        if (m_size * 2 <= m_buckets.size()) {
            return;
        }
        auto save_buckets = std::move(m_buckets);

        m_size = 0;
        m_buckets.resize(save_buckets.size() * 2);
        for (auto &[is_free, data]: save_buckets) {
            if (!is_free) {
                insert(std::move(data));
            }
        }
    }

public:
    hashmap(H hasher = H()) : m_hasher(hasher), m_buckets(8) {}

    // заранее выделяет столько то места для корзины
    hashmap(size_t elements_count, H hasher = H()) : m_hasher(hasher), m_buckets(elements_count) {}

    template<class Iterator>
    hashmap(Iterator first, Iterator last, H hasher = H()) : m_hasher(hasher), m_buckets(8) {
        for (; first != last; first++) {
            insert(*first);
        }
    }

    hashmap(const std::initializer_list<std::pair<K, V>> &l, H hasher = H()) : m_hasher(hasher), m_buckets(8) {
        for (auto &val: l) {
            insert(val);
        }
    }

    [[nodiscard]] size_t size() const {
        return m_size;
    }

    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    void insert(const K &key, V value) {
        std::size_t index = find_index(key); // нашли либо пустую ячейку, либо корзину с этим же ключом

        if (m_buckets[index].is_free) {
            m_size++;
            m_buckets[index].is_free = false;
            m_buckets[index].data.first = key;
        }
        m_buckets[index].data.second = std::move(value);

        update_capacity();
    }

    void insert(const std::pair<K, V> &elem) {
        insert(elem.first, elem.second);
    }

    // вернет правду, если мы удалили
    bool erase(const K &key) {
        std::size_t index = find_index(key);

        if (m_buckets[index].is_free) {
            return false;
        }

        // удалим этот элемент
        m_size--;
        m_buckets[index].is_free = true;

        std::size_t free_pos = index;
        while (true) {
            increase_index(index);

            if (m_buckets[index].is_free) {
                break;
            }

            if (get_index(m_buckets[index].data.first) == index ||
                get_index(m_buckets[index].data.first) > free_pos) {
                continue;
            }

            m_buckets[free_pos].is_free = false;
            m_buckets[free_pos].data = std::move(m_buckets[index].data);

            m_buckets[index].is_free = true;
            free_pos = index;
        }
        return true;
    }

    void clear() {
        for (std::size_t index = 0; index < m_buckets.size(); index++) {
            m_buckets[index].is_free = true;
        }
        m_size = 0;
    }

    V &operator[](const K &key) {
        std::size_t index = find_index(key);
        if (m_buckets[index].is_free) {
            insert(key, V());
            index = find_index(key);
        }
        return m_buckets[index].data.second;
    }

    friend class iterator;

    class iterator {
        std::size_t index = -1;
        const std::vector<bucket> *m_buckets_ptr = nullptr;

    public:
        iterator() = default;

        iterator(std::size_t index, const std::vector<bucket> &buckets) : index(index), m_buckets_ptr(&buckets) {}

        iterator &operator++() {
            index++;
            while (index < m_buckets_ptr->size() && (*m_buckets_ptr)[index].is_free) {
                index++;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator save_it = *this;
            ++(*this);
            return save_it;
        }

        const std::pair<K, V> *operator->() {
            return &(*m_buckets_ptr)[index].data;
        }

        const std::pair<K, V> &operator*() {
            return (*m_buckets_ptr)[index].data;
        }

        bool operator==(const iterator &rhs) const {
            return index == rhs.index && m_buckets_ptr == rhs.m_buckets_ptr;
        }

        bool operator!=(const iterator &rhs) const {
            return index != rhs.index && m_buckets_ptr == rhs.m_buckets_ptr;
        }
    };

    iterator find(const K &key) const {
        std::size_t index = find_index(key);
        if (m_buckets[index].is_free) {
            return end();
        } else {
            return iterator(index, m_buckets);
        }
    }

    iterator begin() const {
        iterator it(-1, m_buckets);
        ++it;
        return it;
    }

    iterator end() const {
        return iterator(m_buckets.size(), m_buckets);
    }

    H hash_function() const {
        return m_hasher;
    }

    // вернет индекс корзины, в которой лежит элемент с таким ключом
    // или -1, если такого элемента нет
    long long bucket(const K &key) const {
        std::size_t index = find_index(key);
        if (m_buckets[index].is_free) {
            return -1;
        } else {
            return static_cast<long long>(index);
        }
    }
};
