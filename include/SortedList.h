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

        Node() : data_(), next_(this), prev_(this) {}
        Node(const T& data) : data_(data), next_(nullptr), prev_(nullptr) {}
    };

    Node sentinel_;
    size_t size_;
    Compare comp_;

public:
    class Iterator {
        Node* curr_;
    public:
        friend class SortedList<T, Compare>;

        Iterator(Node* node = nullptr) : curr_(node) {}
        Iterator(const Iterator& other) : curr_(other.curr_) {}

        T& operator*();
        T* operator->();
        Iterator& operator++() noexcept;
        Iterator operator++(int) noexcept;
        Iterator& operator--() noexcept;
        Iterator operator--(int) noexcept;

        bool operator==(const Iterator& other) const noexcept { return curr_ == other.curr_; }
        bool operator!=(const Iterator& other) const noexcept { return curr_ != other.curr_; }
    };

    class ConstIterator {
        const Node* curr_;
    public:
        friend class SortedList<T, Compare>;

        ConstIterator(const Node* node = nullptr) : curr_(node) {}
        ConstIterator(const ConstIterator& other) : curr_(other.curr_) {}

        const T& operator*() const;
        const T* operator->() const;
        ConstIterator& operator++() noexcept;
        ConstIterator operator++(int) noexcept;
        ConstIterator& operator--() noexcept;
        ConstIterator operator--(int) noexcept;

        bool operator==(const ConstIterator& other) const noexcept { return curr_ == other.curr_; }
        bool operator!=(const ConstIterator& other) const noexcept { return curr_ != other.curr_; }
    };

    SortedList() : sentinel_(), size_(0), comp_() { sentinel_.next_ = sentinel_.prev_ = &sentinel_; }

    template <typename Iter>
    SortedList(Iter first, Iter last) : SortedList() { insert(first, last); }

    SortedList(const std::initializer_list<T>& init) : SortedList(init.begin(), init.end()) {}
    SortedList(const SortedList& other);
    SortedList& operator=(const SortedList& other);
    SortedList(SortedList&& other) noexcept;
    SortedList& operator=(SortedList&& other) noexcept;
    ~SortedList() { clear(); }

    void clear() noexcept;
    bool is_empty() const noexcept { return size_ == 0; }
    size_t size() const noexcept { return size_; }

    T& front();
    const T& front() const;
    T& back();
    const T& back() const;

    Iterator insert(const T& data);
    Iterator insert_unique(const T& data);
    template <typename Iter>
    void insert(Iter first, Iter last);
    void insert_back(const T& data);

    Iterator find(const T& key) noexcept;
    ConstIterator find(const T& key) const noexcept;

    Iterator erase(Iterator pos) noexcept;

    void sort() noexcept;
    void merge_sorted(SortedList&& other) noexcept;
    bool is_sorted() const noexcept;

    ConstIterator begin()  const noexcept { return ConstIterator(sentinel_.next_); }
    ConstIterator end()    const noexcept { return ConstIterator(&sentinel_); }
    ConstIterator cbegin() const noexcept { return begin(); }
    ConstIterator cend()   const noexcept { return end(); }
    Iterator      begin()        noexcept { return Iterator(sentinel_.next_); }
    Iterator      end()          noexcept { return Iterator(&sentinel_); }

private:

	Node* merge_sort(Node* head) noexcept;

};

// ----------------------------------------------------------------------------
// ITERATOR
// ----------------------------------------------------------------------------

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
SortedList<T, Compare>::Iterator::operator++() noexcept {
    curr_ = curr_->next_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::Iterator::operator++(int) noexcept {
    Iterator t = *this;
    ++(*this);
    return t;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator&
SortedList<T, Compare>::Iterator::operator--() noexcept {
    curr_ = curr_->prev_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::Iterator::operator--(int) noexcept {
    Iterator t = *this;
    --(*this);
    return t;
}

// ----------------------------------------------------------------------------
// CONST ITERATOR
// ----------------------------------------------------------------------------

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
SortedList<T, Compare>::ConstIterator::operator++() noexcept {
    curr_ = curr_->next_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::ConstIterator::operator++(int) noexcept {
    ConstIterator t = *this;
    ++(*this);
    return t;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator&
SortedList<T, Compare>::ConstIterator::operator--() noexcept {
    curr_ = curr_->prev_;
    return *this;
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::ConstIterator::operator--(int) noexcept {
    ConstIterator t = *this;
    --(*this);
    return t;
}

// ----------------------------------------------------------------------------
// CONSTRUCTORS AND DESTRUCTOR
// ----------------------------------------------------------------------------

template <typename T, typename Compare>
SortedList<T, Compare>::SortedList(const SortedList& other)
    : sentinel_(), size_(0), comp_(other.comp_) {
    Node* curr = other.sentinel_.next_;
    Node* copy_prev = &sentinel_;

    while (curr != &other.sentinel_) {
        Node* p = new Node(curr->data_);
        copy_prev->next_ = p;
        p->prev_ = copy_prev;
        p->next_ = &sentinel_;
        copy_prev = p;

        curr = curr->next_;
        ++size_;
    }

    sentinel_.prev_ = copy_prev;
    copy_prev->next_ = &sentinel_;
}

template <typename T, typename Compare>
SortedList<T, Compare>& SortedList<T, Compare>::operator=(const SortedList& other) {
    if (this == &other) {
        return *this;
    }
    clear();
    comp_ = other.comp_;
    Node* curr = other.sentinel_.next_;
    Node* copy_prev = &sentinel_;

    while (curr != &other.sentinel_) {
        Node* p = new Node(curr->data_);
        copy_prev->next_ = p;
        p->prev_ = copy_prev;
        p->next_ = &sentinel_;
        copy_prev = p;

        curr = curr->next_;
        ++size_;
    }

    sentinel_.prev_ = copy_prev;
    copy_prev->next_ = &sentinel_;

    return *this;
}

template <typename T, typename Compare>
SortedList<T, Compare>::SortedList(SortedList&& other) noexcept
    : sentinel_(), size_(other.size_), comp_(std::move(other.comp_)) {
    if (!other.is_empty()) {
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

    if (!other.is_empty()) {
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
    sentinel_.next_ = sentinel_.prev_ = &sentinel_;
    size_ = 0;
}


// ----------------------------------------------------------------------------
// BASIC METHODS
// ----------------------------------------------------------------------------

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
    Node* curr = sentinel_.next_;

    while (curr != &sentinel_ && comp_(curr->data_, data)) {
        curr = curr->next_;
    }

    Node* p = new Node(data);
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
void SortedList<T, Compare>::insert_back(const T& data) {
	Node* p = new Node(data);
	p->next_ = &sentinel_;
	p->prev_ = sentinel_.prev_;
	sentinel_.prev_->next_ = p;
	sentinel_.prev_ = p;
	++size_;
}

template<typename T, typename Compare>
template <typename Iter>
void SortedList<T,Compare>::insert(Iter first, Iter last) {
    while (first != last) {
        insert_back(*first++);
    }
	sort();
}

template<typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::find(const T& key) noexcept {
    for (auto it = begin(); it != end(); ++it) {
        if (!comp_(*it, key) && !comp_(key, *it)) {
            return it;
        }
    }
    return end();
}

template<typename T, typename Compare>
typename SortedList<T, Compare>::ConstIterator
SortedList<T, Compare>::find(const T& key) const noexcept {
    for (auto it = cbegin(); it != cend(); ++it) {
        if (!comp_(*it, key) && !comp_(key, *it)) {
            return it;
        }
    }
    return cend();
}

template <typename T, typename Compare>
typename SortedList<T, Compare>::Iterator
SortedList<T, Compare>::erase(Iterator pos) noexcept {
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

template <typename T, typename Compare>
typename SortedList<T, Compare>::Node* 
SortedList<T,Compare>::merge_sort(typename SortedList<T, Compare>::Node* head) noexcept {
    if (!head || head->next_ == &sentinel_) {
        return head;
    }

    Node* slow = head;
    Node* fast = head->next_;
    while (fast != &sentinel_ && fast->next_ != &sentinel_) {
        slow = slow->next_;
        fast = fast->next_->next_;
    }

    Node* mid = slow->next_;
    slow->next_ = &sentinel_;
    mid->prev_ = &sentinel_;
    Node* left = merge_sort(head);
    Node* right = merge_sort(mid);

    Node fict;
    Node* tail = &fict;
    while (left != &sentinel_ && right != &sentinel_) {
        if (comp_(left->data_, right->data_)) {
            tail->next_ = left;
            left->prev_ = tail;
            left = left->next_;
        }
        else {
            tail->next_ = right;
            right->prev_ = tail;
            right = right->next_;
        }
        tail = tail->next_;
    }
    if (left != &sentinel_) {
        tail->next_ = left;
        left->prev_ = tail;
    }
    else {
        tail->next_ = right;
        right->prev_ = tail;
    }
    return fict.next_;
}


template <typename T, typename Compare>
void SortedList<T, Compare>::sort() noexcept {
    if (size_ <= 1) {
        return;
    }
    Node* new_head = merge_sort(sentinel_.next_);
    sentinel_.next_ = new_head;
    new_head->prev_ = &sentinel_;

    Node* curr = new_head;
    while (curr->next_ != &sentinel_) {
        curr = curr->next_;
    }
    sentinel_.prev_ = curr;
    curr->next_ = &sentinel_;
}

template <typename T, typename Compare>
void SortedList<T, Compare>::merge_sorted(SortedList&& other) noexcept {
    if (this == &other) {
        return;
    }
    Node* this_curr = sentinel_.next_;
    Node* other_curr = other.sentinel_.next_;
    while (this_curr != &sentinel_ && other_curr != &other.sentinel_) {
        if (comp_(other_curr->data_, this_curr->data_)) {
            Node* next_other = other_curr->next_;
            other_curr->prev_ = this_curr->prev_;
            other_curr->next_ = this_curr;
            this_curr->prev_->next_ = other_curr;
            this_curr->prev_ = other_curr;
            other_curr = next_other;
            ++size_;
            --other.size_;
        }
        else {
            this_curr = this_curr->next_;
        }
    }
    while (other_curr != &other.sentinel_) {
        Node* next_other = other_curr->next_;
        other_curr->prev_ = sentinel_.prev_;
        other_curr->next_ = &sentinel_;
        sentinel_.prev_->next_ = other_curr;
        sentinel_.prev_ = other_curr;
        other_curr = next_other;
        ++size_;
        --other.size_;
    }
    other.sentinel_.next_ = &other.sentinel_;
    other.sentinel_.prev_ = &other.sentinel_;
}   

template <typename T, typename Compare>
bool SortedList<T, Compare>::is_sorted() const noexcept {
    if (size_ <= 1) {
        return true;
    }
    Node* curr = sentinel_.next_;
    while (curr->next_ != &sentinel_) {
        if (comp_(curr->next_->data_, curr->data_)) {
            return false;
        }
        curr = curr->next_;
    }
    return true;
}