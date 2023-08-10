#pragma once

#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template <class K, class V, class H = std::hash<K>>
class hashmap {
    // first is key
    // second is value
    using Item = std::pair<K, V>;
    using Bucket = std::optional<Item>;

    H m_hasher;

    std::vector<Bucket> m_buckets;

    std::size_t m_size = 0;

    //=================//
    //==INDEX METHODS==//
    //=================//

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
        for (; m_buckets[index] && !(m_buckets[index]->first == key);
             increase_index(index)) {
        }
        return index;
    }

    //===========================//
    //==INTERNAL CHANGE METHODS==//
    //===========================//

    // при необходимости расширяет корзину, чтобы поддерживать быстрый доступ к
    // элементам
    void update_capacity() {
        if (1 + m_size * 2 <= m_buckets.size()) {
            return;
        }
        auto old_buckets = std::move(m_buckets);

        m_size = 0;
        m_buckets.resize(old_buckets.size() * 2);

        for (auto &old_bucket : old_buckets) {
            if (old_bucket) {
                insert(std::move(*old_bucket));
            }
        }
    }

public:
    //============//
    //==TYPEDEFS==//
    //============//

    using key_type = K;
    using mapped_type = V;
    using value_type = std::pair<K, V>;
    using hasher = H;

    using pointer = value_type *;
    using const_pointer = const value_type *;

    using reference = value_type &;
    using const_reference = const value_type &;

    using size_type = std::size_t;
    using difference_type = long long;

    friend class iterator;
    friend class const_iterator;

    //================//
    //==CONSTRUCTORS==//
    //================//

    hashmap(H hasher = H()) : m_hasher(hasher), m_buckets(8) {
    }

    // заранее выделяет столько то места для корзины
    hashmap(std::size_t elements_count, H hasher = H())
        : m_hasher(hasher), m_buckets(2 * elements_count) {
    }

    template <class Iterator>
    hashmap(Iterator first, Iterator last, H hasher = H())
        : hashmap(2 * std::distance(first, last), hasher) {
        for (; first != last; first++) {
            insert(*first);
        }
    }

    hashmap(const std::initializer_list<Item> &l, H hasher = H())
        : hashmap(l.begin(), l.end(), hasher) {
    }

    hashmap(hashmap &&other) noexcept
        : m_size(std::exchange(other.m_size, 0)),
          m_buckets(std::move(other.m_buckets)),
          m_hasher(std::move(other.m_hasher)) {
        other.m_buckets.resize(8);
    }

    hashmap(const hashmap &other) : hashmap(other.size(), other.m_hasher) {
        for (auto it : other) {
            insert(it);
        }
    }

    hashmap &operator=(const hashmap &other) {
        if (this != &other) {
            clear();
            for (auto value : other) {
                insert(value);
            }
        }
        return *this;
    }

    hashmap &operator=(hashmap &&other) noexcept {
        if (this != &other) {
            clear();
            m_size = std::exchange(other.m_size, 0);
            m_buckets = std::move(other.m_buckets);
            m_hasher = std::move(other.m_hasher);
            other.m_buckets.resize(8);
        }
        return *this;
    }

    //===========//
    //==GETTERS==//
    //===========//

    [[nodiscard]] std::size_t size() const {
        return m_size;
    }

    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    // хеш-функцию нельзя изменять: нарушиться кучу свойств
    // поэтому передается по const&
    const H &hash_function() const {
        return m_hasher;
    }

    template <typename T = float>
    T load_factor() const {
        return static_cast<T>(size()) / m_buckets.size();
    }

    //============//
    //==ITERATOR==//
    //============//

    class iterator {
        std::size_t index = -1;
        std::vector<Bucket> &m_buckets;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = hashmap::value_type;
        using pointer = hashmap::pointer;
        using reference = hashmap::reference;
        using iterator_category = std::forward_iterator_tag;

        iterator(std::size_t index, std::vector<Bucket> &buckets)
            : index(index), m_buckets(buckets) {
        }

        iterator &operator++() {
            index++;
            while (index < m_buckets.size() && !m_buckets[index]) {
                index++;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator save_it = *this;
            ++(*this);
            return save_it;
        }

        pointer operator->() {
            return &*(m_buckets)[index];
        }

        reference operator*() {
            return *(m_buckets)[index];
        }

        friend bool operator==(const iterator &lhs, const iterator &rhs) {
            return lhs.index == rhs.index && lhs.m_buckets == rhs.m_buckets;
        }

        friend bool operator!=(const iterator &lhs, const iterator &rhs) {
            return !(lhs == rhs);
        }
    };

    class const_iterator {
        std::size_t index = -1;
        const std::vector<Bucket> &m_buckets = nullptr;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = hashmap::value_type;
        using pointer = hashmap::const_pointer;
        using reference = hashmap::const_reference;
        using iterator_category = std::forward_iterator_tag;

        const_iterator(std::size_t index, const std::vector<Bucket> &buckets)
            : index(index), m_buckets(buckets) {
        }

        const_iterator &operator++() {
            index++;
            while (index < m_buckets.size() && !m_buckets[index]) {
                index++;
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator save_it = *this;
            ++(*this);
            return save_it;
        }

        pointer operator->() const {
            return &*(m_buckets)[index];
        }

        reference operator*() const {
            return *(m_buckets)[index];
        }

        friend bool
        operator==(const const_iterator &lhs, const const_iterator &rhs) {
            return lhs.index == rhs.index && lhs.m_buckets == rhs.m_buckets;
        }

        friend bool
        operator!=(const const_iterator &lhs, const const_iterator &rhs) {
            return !(lhs == rhs);
        }
    };

    iterator begin() {
        iterator it(-1, m_buckets);
        ++it;  // продвигаем до первого элемента
        return it;
    }

    const_iterator begin() const {
        return cbegin();
    }

    const_iterator cbegin() const {
        const_iterator it(-1, m_buckets);
        ++it;  // продвигаем до первого элемента
        return it;
    }

    iterator end() {
        return iterator(m_buckets.size(), m_buckets);
    }

    const_iterator end() const {
        return cend();
    }

    const_iterator cend() const {
        return const_iterator(m_buckets.size(), m_buckets);
    }

    //=============//
    //==MODIFIERS==//
    //=============//

private:
    std::pair<std::size_t, bool> insert_item(Item item) {
        update_capacity();
        std::size_t index = find_index(item.first);
        bool need_insert = !m_buckets[index];
        if (need_insert) {
            m_size++;
            m_buckets[index].emplace(
                std::move(item.first), std::move(item.second)
            );
        }
        return std::make_pair(index, need_insert);
    }

public:
    template <typename Item = std::pair<K, V>>
    std::pair<iterator, bool> insert(Item &&item) {
        auto [index, was_inserted] = insert_item(std::forward<Item>(item));
        return std::make_pair(iterator(index, m_buckets), was_inserted);
    }

    void insert(std::initializer_list<Item> item_list) {
        for (auto &item : item_list) {
            insert(std::move(item));
        }
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args) {
        return insert(Item(std::forward<Args>(args)...));
    }

    // вернет правду, если мы удалили
    bool erase(const K &key) {
        std::size_t index = find_index(key);

        // если такого элемента нет
        if (!m_buckets[index]) {
            return false;
        }

        // удалим этот элемент
        m_size--;
        m_buckets[index].reset();

        // аккуратно переместим все элементы после удаленного так, чтобы в итоге
        // получили, как если бы никогда не было этого элемента

        // свободное место
        std::size_t free_pos = index;

        while (true) {
            increase_index(index);

            // дошли до пустого
            if (!m_buckets[index]) {
                break;  // закончили с перемещениями
            }

            // если этот элемент уже стоит на нужном месте или он должен стоять
            // правее чем мы хотим его поставить
            std::size_t need_index = get_index(m_buckets[index]->first);
            if (need_index == index || need_index > free_pos) {
                continue;  // мы не должны его трогать
            }

            m_buckets[free_pos] = std::move(m_buckets[index]);
            m_buckets[index].reset();

            free_pos = index;
        }
        return true;
    }

    void clear() {
        for (std::size_t index = 0; index < m_buckets.size(); index++) {
            m_buckets[index].reset();
        }
        m_size = 0;
    }

    V &operator[](const K &key) {
        std::size_t index = find_index(key);
        if (!m_buckets[index]) {
            index = insert_item(Item(key, V())).first;
        }
        return m_buckets[index]->second;
    }

    V &operator[](K &&key) {
        std::size_t index = find_index(key);
        if (!m_buckets[index]) {
            index = insert_item(Item(std::move(key), V())).first;
        }
        return m_buckets[index]->second;
    }

    //==========//
    //==LOOKUP==//
    //==========//

    // вернет индекс корзины, в которой лежит элемент с таким ключом
    // или -1, если такого элемента нет
    long long bucket(const K &key) const {
        std::size_t index = find_index(key);
        if (!m_buckets[index]) {
            return -1;
        } else {
            return static_cast<long long>(index);
        }
    }

    iterator find(const K &key) {
        long long index = bucket(key);
        if (index == -1) {
            return end();
        } else {
            return iterator(index, m_buckets);
        }
    }

    const_iterator find(const K &key) const {
        long long index = bucket(key);
        if (index == -1) {
            return end();
        } else {
            return const_iterator(index, m_buckets);
        }
    }

private:
    std::size_t at_index(const K &key) const {
        long long index = bucket(key);
        if (index == -1) {
            throw std::out_of_range("failed hashmap::at(key)");
        }
        return index;
    }

public:
    V &at(const K &key) {
        return m_buckets[at_index(key)]->second;
    }

    const V &at(const K &key) const {
        return m_buckets[at_index(key)]->second;
    }

    bool contains(const K &key) const {
        return bucket(key) != -1;
    }

    std::size_t count(const K &key) const {
        return contains(key);
    }

    friend bool operator==(const hashmap &lhs, const hashmap &rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        } else {
            return std::is_permutation(lhs.begin(), lhs.end(), rhs.begin());
        }
    }
};
