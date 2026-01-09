#pragma once

#include <functional>
#include <stdexcept>
#include <initializer_list>
#include <type_traits>

template <typename T, typename Compare = std::less<T>>
class SortedList {
private:
    struct Node {
        T data_;
        Node* next_;
        Node* prev_;

        Node();
        Node(const T& data);
    };

    Node sentinel_;
    size_t size_;
    Compare comp_;

public:
    class Iterator {
        Node* curr_;
    public:
        friend class SortedList<T, Compare>;

        Iterator(Node* node = nullptr);
        Iterator(const Iterator& other);

        T& operator*();
        T* operator->();
        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);

        bool operator==(const Iterator& other) const noexcept;
        bool operator!=(const Iterator& other) const noexcept;
    };

    class ConstIterator {
        const Node* curr_;
    public:
        friend class SortedList<T, Compare>;

        ConstIterator(const Node* node = nullptr);
        ConstIterator(const ConstIterator& other);

        const T& operator*() const;
        const T* operator->() const;
        ConstIterator& operator++();
        ConstIterator operator++(int);
        ConstIterator& operator--();
        ConstIterator operator--(int);

        bool operator==(const ConstIterator& other) const noexcept;
        bool operator!=(const ConstIterator& other) const noexcept;
    };

    SortedList(); 

    template <typename Iter>
    SortedList(Iter first, Iter last);

    SortedList(std::initializer_list<T> init);
    SortedList(const SortedList& other);
    SortedList& operator=(const SortedList& other);
    SortedList(SortedList&& other) noexcept;
    SortedList& operator=(SortedList&& other) noexcept;
    ~SortedList();

    void clear() noexcept;
    bool is_empty() const noexcept;
    size_t size() const noexcept;

    T& front();
    const T& front() const;
    T& back();
    const T& back() const;

    Iterator insert(const T& data);
    Iterator insert_unique(const T& data);

    template <typename Iter>
    void insert(Iter first, Iter last);

    Iterator find(const T& key);
    ConstIterator find(const T& key) const;

    Iterator erase(Iterator pos);

    ConstIterator begin() const;
    ConstIterator end() const;
    ConstIterator cbegin() const;
    ConstIterator cend() const;
    Iterator begin();
    Iterator end();
};


// ----------------------------------------------------------------------------
//  NODE
// ----------------------------------------------------------------------------

template <typename T, typename Compare>
SortedList<T, Compare>::Node::Node()
    : data_(), next_(this), prev_(this) {
}

template <typename T, typename Compare>
SortedList<T, Compare>::Node::Node(const T& data_)
    : data_(data_), next_(nullptr), prev_(nullptr) {
}

// ----------------------------------------------------------------------------
// ITERATOR
// ----------------------------------------------------------------------------

template <typename T, typename Compare>
SortedList<T, Compare>::Iterator::Iterator(Node* node)
    : curr_(node) {
}

template <typename T, typename Compare>
SortedList<T, Compare>::Iterator::Iterator(const Iterator& other)
    : curr_(other.curr_) {
}

template <typename T, typename Compare>
T& SortedList<T, Compare>::Iterator::operator*() {
    if (!curr_ || curr_ == curr_->next_) {
        throw std::runtime_error("Dereferencing end iterator");
    }
    return curr_->data_;
}

template <typename T, typename Compare>
T* SortedList<T, Compare>::Iterator::operator->() {
    if (!curr_ || curr_ == curr_->next_) {
        throw std::runtime_error("Accessing end iterator");
    }
    return &curr_->data_;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator&
SortedList<T, Compare>::Iterator::operator++() {
    curr_ = curr_->next_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::Iterator::operator++(int) {
    Iterator t = *this;
    ++(*this);
    return t;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator&
SortedList<T, Compare>::Iterator::operator--() {
    curr_ = curr_->prev_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::Iterator::operator--(int) {
    Iterator t = *this;
    --(*this);
    return t;
}

template <typename T, typename Compare>
bool SortedList<T, Compare>::Iterator::operator==(const Iterator& other) const noexcept {
    return curr_ == other.curr_;
}

template <typename T, typename Compare>
bool SortedList<T, Compare>::Iterator::operator!=(const Iterator& other) const noexcept {
    return curr_ != other.curr_;
}

// ----------------------------------------------------------------------------
// CONST ITERATOR
// ----------------------------------------------------------------------------

template <typename T, typename Compare>
SortedList<T, Compare>::ConstIterator::ConstIterator(const Node* node)
    : curr_(node) {
}

template <typename T, typename Compare>
SortedList<T, Compare>::ConstIterator::ConstIterator(const ConstIterator& other)
    : curr_(other.curr_) {
}

template <typename T, typename Compare>
const T& SortedList<T, Compare>::ConstIterator::operator*() const {
    if (!curr_ || curr_->next_ == curr_) {
        throw std::runtime_error("Dereferencing end const-iterator");
    }
    return curr_->data_;
}

template <typename T, typename Compare>
const T* SortedList<T, Compare>::ConstIterator::operator->() const {
    if (!curr_ || curr_->next_ == curr_) {
        throw std::runtime_error("Accessing end const-iterator");
    }
    return &curr_->data_;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator&
SortedList<T, Compare>::ConstIterator::operator++() {
    curr_ = curr_->next_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::ConstIterator::operator++(int) {
    ConstIterator t = *this;
    ++(*this);
    return t;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator&
SortedList<T, Compare>::ConstIterator::operator--() {
    curr_ = curr_->prev_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::ConstIterator::operator--(int) {
    ConstIterator t = *this;
    --(*this);
    return t;
}

template <typename T, typename Compare>
bool SortedList<T, Compare>::ConstIterator::operator==(const ConstIterator& other) const noexcept {
    return curr_ == other.curr_;
}

template <typename T, typename Compare>
bool SortedList<T, Compare>::ConstIterator::operator!=(const ConstIterator& other) const noexcept {
    return curr_ != other.curr_;
}

// ----------------------------------------------------------------------------
// CONSTRUCTORS AND DESTRUCTOR
// ----------------------------------------------------------------------------

template <typename T, typename Compare>
SortedList<T, Compare>::SortedList()
    : sentinel_(), size_(0), comp_() {
    sentinel_.next_ = sentinel_.prev_ = &sentinel_;
}

template <typename T, typename Compare>
template <typename Iter>
SortedList<T, Compare>::SortedList(Iter first, Iter last)
    : SortedList() {
    insert(first, last);
}

template <typename T, typename Compare>
SortedList<T, Compare>::SortedList(std::initializer_list<T> init)
    : SortedList(init.begin(), init.end()) {
}

template <typename T, typename Compare>
SortedList<T, Compare>::SortedList(const SortedList& other)
    : SortedList() {
    for (const auto& item : other) {
        insert(item);
    }
}

template <typename T, typename Compare>
SortedList<T, Compare>& SortedList<T, Compare>::operator=(const SortedList& other) {
    if (this == &other) {
        return *this;
    }
    clear();
    comp_ = other.comp_;
    for (const auto& item : other) {
        insert(item);
    }
    return *this;
}

template <typename T, typename Compare>
SortedList<T, Compare>::SortedList(SortedList&& other) noexcept
    : sentinel_(), size_(other.size_), comp_(std::move(other.comp_)) {
    if (other.size_ != 0) {
        sentinel_.next_ = other.sentinel_.next_;
        sentinel_.prev_ = other.sentinel_.prev_;
        sentinel_.next_->prev_ = &sentinel_;
        sentinel_.prev_->next_ = &sentinel_;

        other.sentinel_.next_ = &other.sentinel_;
        other.sentinel_.prev_ = &other.sentinel_;
        other.size_ = 0;
    }
}

template <typename T, typename Compare>
SortedList<T, Compare>& SortedList<T, Compare>::operator=(SortedList&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    clear();

    size_ = other.size_;
    comp_ = std::move(other.comp_);

    if (other.size_ != 0) {
        sentinel_.next_ = other.sentinel_.next_;
        sentinel_.prev_ = other.sentinel_.prev_;
        sentinel_.next_->prev_ = &sentinel_;
        sentinel_.prev_->next_ = &sentinel_;
    }

    other.sentinel_.next_ = &other.sentinel_;
    other.sentinel_.prev_ = &other.sentinel_;
    other.size_ = 0;

    return *this;
}

template <typename T, typename Compare>
void SortedList<T, Compare>::clear() noexcept {
    Node* curr = sentinel_.next_;
    while (curr != &sentinel_) {
        Node* next = curr->next_;
        delete curr;
        curr = next;
    }
    sentinel_.next_ = &sentinel_;
    sentinel_.prev_ = &sentinel_;
    size_ = 0;
}

template <typename T, typename Compare>
SortedList<T, Compare>::~SortedList() {
    clear();
}

// ----------------------------------------------------------------------------
// BASIC METHODS
// ----------------------------------------------------------------------------

template <typename T, typename Compare>
bool SortedList<T, Compare>::is_empty() const noexcept {
    return size_ == 0;
}

template <typename T, typename Compare>
size_t SortedList<T, Compare>::size() const noexcept {
    return size_;
}

template <typename T, typename Compare>
T& SortedList<T, Compare>::front() {
    if (is_empty()) {
        throw std::runtime_error("List is empty");
    }
    return sentinel_.next_->data_;
}

template <typename T, typename Compare>
const T& SortedList<T, Compare>::front() const {
    if (is_empty()) {
        throw std::runtime_error("List is empty");
    }
    return sentinel_.next_->data_;
}

template <typename T, typename Compare>
T& SortedList<T, Compare>::back() {
    if (is_empty()) {
        throw std::runtime_error("List is empty");
    }
    return sentinel_.prev_->data_;
}

template <typename T, typename Compare>
const T& SortedList<T, Compare>::back() const {
    if (is_empty()) {
        throw std::runtime_error("List is empty");
    }
    return sentinel_.prev_->data_;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::insert(const T& data) {
    Node* p = new Node(data);
    Node* curr = sentinel_.next_;

    while (curr != &sentinel_ && comp_(curr->data_, data)) {
        curr = curr->next_;
    }

    p->next_ = curr;
    p->prev_ = curr->prev_;
    curr->prev_->next_ = p;
    curr->prev_ = p;

    ++size_;
    return Iterator(p);
}

template <typename T, typename Compare>
typename SortedList<T,Compare>::Iterator
SortedList<T, Compare>::insert_unique(const T& data) {
    if (is_empty()) {
        insert(data);
    }
    Node* curr = sentinel_.next_;
    while (curr != &sentinel_ && comp_(curr->data_, data)) {
        curr = curr->next_;
    }
    if (curr != &sentinel_ && !comp_(data, curr->data_) && !comp_(curr->data_, data)) {
        return Iterator(curr);
    }
    Node* p = new Node(data);
    p->next_ = curr;
    p->prev_ = curr->prev_;
    curr->prev_->next_ = p;
    curr->prev_ = p;

    ++size_;
    return Iterator(p);
}

template<typename T, typename Compare>
template <typename Iter>
void SortedList<T,Compare>::insert(Iter first, Iter last) {
    while (first != last) {
        insert(*first++);
    }
}

template<typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::find(const T& key) {
    for (auto it = begin(); it != end(); ++it) {
        if (!comp_(*it, key) && !comp_(key, *it)) {
            return it;
        }
    }
    return end();
}

template<typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::find(const T& key) const {
    for (auto it = cbegin(); it != cend(); ++it) {
        if (!comp_(*it, key) && !comp_(key, *it)) {
            return it;
        }
    }
    return cend();
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::erase(Iterator pos) {
    if (pos.curr_ == &sentinel_) {
        return end();
    }

    Node* p = pos.curr_;
    Iterator next_it(p->next_);

    p->prev_->next_ = p->next_;
    p->next_->prev_ = p->prev_;

    delete p;
    pos.curr_ = nullptr;
    --size_;

    return next_it;
}

// ----------------------------------------------------------------------------
// ITERATORS
// ----------------------------------------------------------------------------

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::begin() const {
    return ConstIterator(sentinel_.next_);
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::end() const {
    return ConstIterator(&sentinel_);
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::cbegin() const {
    return begin();
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::cend() const {
    return end();
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::begin() {
    return Iterator(sentinel_.next_);
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::end() {
    return Iterator(&sentinel_);
}
