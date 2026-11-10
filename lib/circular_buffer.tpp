#pragma once

#include "circular_buffer.h"
#include <utility>

// container members

template<typename T, typename Allocator> 
void circular_buffer<T, Allocator>::swap(circular_buffer& other) noexcept{
    std::swap(alloc_, other.alloc_);      
    std::swap(data_, other.data_);         
    std::swap(capacity_, other.capacity_);   
    std::swap(size_, other.size_);       
    std::swap(head_, other.head_); 
}

template<typename T, typename Allocator> 
void circular_buffer<T, Allocator>::clear() noexcept{
    for (size_type i = 0; i < size_; ++i){
        alloc_traits::destroy(alloc_, data_ + normalize_index(i));
    }
    head_ = 0;
    size_ = 0;
}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>::circular_buffer(const Allocator& alloc)
    : alloc_(alloc), capacity_(0), size_(0), head_(0) {
    data_ = alloc_traits::allocate(alloc_, capacity_);
}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>::circular_buffer(size_type count, const Allocator& alloc)
    : alloc_(alloc), capacity_(count), size_(0), head_(0) {
    data_ = alloc_traits::allocate(alloc_, capacity_);
}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>::circular_buffer(const circular_buffer& other)
    : alloc_(alloc_traits::select_on_container_copy_construction(other.alloc_)), 
    capacity_(other.capacity_), size_(other.size_), head_(0) {

    data_ = alloc_traits::allocate(alloc_, capacity_);

    for (size_type i = 0; i < size_; ++i)
        alloc_traits::construct(alloc_, data_ + i, other.data_ + other.normalize_index(i));

}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>::circular_buffer(const circular_buffer& other, const Allocator& alloc)
    : alloc_(alloc), capacity_(other.capacity_), size_(other.size_), head_(0) {
    
    data_ = alloc_traits::allocate(alloc_, capacity_);

    for (size_type i = 0; i < size_; ++i){
        alloc_traits::construct(alloc_, data_ + i, other.data_ + other.normalize_index(i)); 
    }
}

template<typename T, typename Allocator>
template<class InputIterator>
circular_buffer<T, Allocator>::circular_buffer(InputIterator first, InputIterator last, 
                                                size_type capacity, const Allocator& alloc)
    : alloc_(alloc), capacity_(capacity), size_(0), head_(0) {

    data_ = alloc_traits::allocate(alloc_, capacity_);

    for (size_type i = 0; i < capacity_ && first != last; ++i, ++first, ++size_) 
        alloc_traits::construct(alloc_, data_ + i, *first);
}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>::circular_buffer(std::initializer_list<T> init, 
                                               const Allocator& alloc)
    : circular_buffer(init.size(), alloc) {
    auto it = init.begin();
    for (size_type i = 0; i < init.size(); ++i) {
        alloc_traits::construct(alloc_, data_ + i, *it++);
        ++size_;
    }
}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>::circular_buffer(size_type count, 
    const T& value, const Allocator& alloc)
    : alloc_(alloc), capacity_(count), size_(count), head_(0) {

    data_ = alloc_traits::allocate(alloc_, capacity_);

    for (size_type i = 0; i < capacity_; ++i)
        alloc_traits::construct(alloc_, data_ + i, value);

}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>& 
circular_buffer<T, Allocator>::operator=(const circular_buffer& other) {
    if (this != &other) {
        circular_buffer tmp = circular_buffer(other);
        swap(tmp);
    }
    return *this;
}

template<typename T, typename Allocator>
circular_buffer<T, Allocator>::~circular_buffer(){  
    clear();
    if (data_) alloc_traits::deallocate(alloc_, data_, capacity_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::allocator_type 
circular_buffer<T, Allocator>::get_allocator() const noexcept {
    return alloc_;
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::reference 
circular_buffer<T, Allocator>::front() {
    return data_[head_];
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::reference 
circular_buffer<T, Allocator>::back() {
    return data_[normalize_index(head_ + size_ - 1)];
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reference 
circular_buffer<T, Allocator>::front() const  {
    return data_[head_];
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reference 
circular_buffer<T, Allocator>::back() const {
    return data_[normalize_index(head_ + size_ - 1)];
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::reference 
circular_buffer<T, Allocator>::operator[](size_type pos){
    return data_[normalize_index(pos)];
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reference 
circular_buffer<T, Allocator>::operator[](size_type pos) const{
    return data_[normalize_index(pos)];
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::reference 
circular_buffer<T, Allocator>::at(size_type pos){
    if (pos >= size_) throw std::out_of_range("index out of range");;
    return data_[normalize_index(pos)];
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reference 
circular_buffer<T, Allocator>::at(size_type pos) const{
    if (pos >= size_) throw std::out_of_range("index out of range");;
    return data_[normalize_index(pos)];
}

template<typename T, typename Allocator>
bool circular_buffer<T, Allocator>::empty() const noexcept{
    if (size_) return false;
    return true;
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::size_type 
circular_buffer<T, Allocator>::size() const noexcept{
    return size_;
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::size_type
circular_buffer<T, Allocator>::max_size() const noexcept {
    return alloc_traits::max_size(alloc_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::size_type 
circular_buffer<T, Allocator>::capacity() const noexcept{
    return capacity_;
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::reserve(size_type capacity_n) {

    circular_buffer tmp(capacity_n);

    for(size_type i = 0; i < size_; ++i) 
        alloc_traits::construct(alloc_, tmp.data_ + i, data_[normalize_index(i)]);
    
    tmp.size_ = size_;

    swap(tmp);
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::push_back(const value_type& value) {
    if (size_ == capacity_) reserve(capacity_ * 2 + 1);
    alloc_traits::construct(alloc_, data_ + normalize_index(size_), value);
    ++size_;
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::pop_back() {
    if (size_ == head_) return;
    --size_;
    alloc_traits::destroy(alloc_, data_ + normalize_index(head_ + size_));
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::push_front(const value_type& value) {
    if (size_ == capacity_) reserve(capacity_ * 2 + 1);
    head_ = (head_ - 1 + capacity_) % capacity_;
    alloc_traits::construct(alloc_, data_ + head_, value);
    ++size_;
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::pop_front() {
    if (size_ == head_) return;
    alloc_traits::destroy(alloc_, data_ + head_);
    head_ = (head_ + 1) % capacity_;
    --size_;
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::assign(size_type count, const T& value) {
    circular_buffer tmp(count, value, alloc_);
    swap(tmp);
}

template<typename T, typename Allocator>
template<class InputIt>
void circular_buffer<T, Allocator>::assign(InputIt first, InputIt last) {
    circular_buffer tmp(alloc_);
    while (first != last) tmp.push_back(*first++);
    swap(tmp);
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::assign(std::initializer_list<value_type> ilist) {
    assign(ilist.begin(), ilist.end());
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::iterator 
circular_buffer<T, Allocator>::begin() noexcept{
    return iterator(data_, capacity_, 0, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_iterator 
circular_buffer<T, Allocator>::begin() const noexcept{
    return const_iterator(data_, capacity_, 0, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_iterator 
circular_buffer<T, Allocator>::cbegin() const noexcept{
    return const_iterator(data_, capacity_, 0, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::iterator 
circular_buffer<T, Allocator>::end() noexcept{
    return iterator(data_, capacity_, size_, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_iterator 
circular_buffer<T, Allocator>::end() const noexcept{
    return const_iterator(data_, capacity_, size_, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_iterator 
circular_buffer<T, Allocator>::cend() const noexcept{
    return const_iterator(data_, capacity_, size_, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::reverse_iterator 
circular_buffer<T, Allocator>::rbegin() noexcept{
    return reverse_iterator(data_, capacity_, size_, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reverse_iterator 
circular_buffer<T, Allocator>::rbegin() const noexcept{
    return const_reverse_iterator(data_, capacity_, size_, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reverse_iterator 
circular_buffer<T, Allocator>::crbegin() const noexcept{
    return const_reverse_iterator(data_, capacity_, size_, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::reverse_iterator 
circular_buffer<T, Allocator>::rend() noexcept{
    return reverse_iterator(data_, capacity_, 0, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reverse_iterator 
circular_buffer<T, Allocator>::rend() const noexcept{
    return const_reverse_iterator(data_, capacity_, 0, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::const_reverse_iterator 
circular_buffer<T, Allocator>::crend() const noexcept{
    return const_reverse_iterator(data_, capacity_, 0, head_, size_);
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::iterator
circular_buffer<T, Allocator>::insert(const_iterator pos, const T& value) {
    difference_type index = pos - begin();

    if (index < 0 || index > size_) throw std::out_of_range("insert position out of range");
    if (size_ == capacity_) reserve(capacity_ * 2 + 1);
    
    for (difference_type i = size_; i > index; --i) {
        size_type src = normalize_index(i - 1);
        size_type dst = normalize_index(i);
        
        if (i == size_) alloc_traits::construct(alloc_, data_ + dst, data_[src]);
        else data_[dst] = data_[src];
    }

    alloc_traits::construct(alloc_, data_ + normalize_index(index), value);
    ++size_;
    return begin() + index;
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::iterator
circular_buffer<T, Allocator>::insert(const_iterator pos, size_type count, const T& value) {
    if (count == 0) return begin() + (pos - begin());
    
    difference_type index = pos - begin();
    if (index < 0 || index > size_) throw std::out_of_range("insert position out of range");
    while (size_ + count > capacity_) reserve(capacity_ * 2 + 1);
    
    for (difference_type i = size_ + count - 1; i >= index + count; --i) {
        size_type src = normalize_index(i - 1);
        size_type dst = normalize_index(i);
        if (static_cast<size_type>(i) >= size_) alloc_traits::construct(alloc_, data_ + dst, data_[src]);
        else data_[dst] = data_[src];
    }

    for (size_type i = 0; i < count; ++i) alloc_traits::
        construct(alloc_, data_ + normalize_index(index + i), value);

    size_ += count;
    return begin() + index;
}

template<typename T, typename Allocator>
template<class InputIt, typename>
typename circular_buffer<T, Allocator>::iterator
circular_buffer<T, Allocator>::insert(const_iterator pos, InputIt first, InputIt last) {

    size_type count = 0;
    for (auto it = first; it != last; ++it) ++count;
    if (count == 0) return begin() + (pos - begin());
    
    difference_type index = pos - begin();
    if (index < 0 || index > size_) throw std::out_of_range("insert position out of range");
    
    while (size_ + count > capacity_) reserve(capacity_ * 2 + 1);
    
    for (difference_type i = size_ + count - 1; i >= (index + count); --i) {
        size_type src = normalize_index(i - count);
        size_type dst = normalize_index(i);
        
        if (static_cast<size_type>(i) >= size_)
            alloc_traits::construct(alloc_, data_ + dst, data_[src]);
        else data_[dst] = data_[src];   
    }

    for (size_type i = 0; i < count; ++i, ++first) 
        alloc_traits::construct(alloc_, data_ + normalize_index(index + i), *first);
    
    size_ += count;
    return begin() + index;
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::iterator
circular_buffer<T, Allocator>::insert(const_iterator pos, std::initializer_list<T> ilist) {
    return insert(pos, ilist.begin(), ilist.end());
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::iterator
circular_buffer<T, Allocator>::erase(const_iterator pos) {
    difference_type index = pos - begin();
    if (index < 0 || index >= size_) return end();

    alloc_traits::destroy(alloc_, data_ + normalize_index(index));
    
    for (difference_type i = index; i < (size_ - 1); ++i) {
        size_type src = normalize_index(i + 1);
        size_type dst = normalize_index(i);
        data_[dst] = data_[src];
    }

    alloc_traits::destroy(alloc_, data_ + normalize_index(size_ - 1));    
    --size_;
    return begin() + index;
}

template<typename T, typename Allocator>
typename circular_buffer<T, Allocator>::iterator
circular_buffer<T, Allocator>::erase(const_iterator first, const_iterator last) {
    if (first == last) return begin() + (first - begin());
    
    difference_type start_index = first - begin();
    difference_type end_index = last - begin();
    size_type count = end_index - start_index;
    
    if (start_index < 0 || end_index > size_) return end();
    
    for (difference_type i = start_index; i < end_index; ++i) 
        alloc_traits::destroy(alloc_, data_ + normalize_index(i));

    for (difference_type i = end_index; i < size_; ++i) {
        size_type src = normalize_index(i);
        size_type dst = normalize_index(i - count);
        data_[dst] = data_[src];
    }
    
    for (size_type i = size_ - count; i < size_; ++i)
        alloc_traits::destroy(alloc_, data_ + normalize_index(i));
    
    size_ -= count;
    return begin() + start_index;
}

template<typename T, typename Allocator>
void circular_buffer<T, Allocator>::resize(size_type count, const value_type& value) {
    if (count < size_) {
        while (size_ > count) pop_back();   
    } 
    else if (count > size_) {
        while (size_ < count) push_back(value);
    }
}


// iterator members


template<typename T, typename Pointer, typename Reference, bool IsReverse>
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
circular_buffer_iterator(pointer data, size_t cap, size_t index, 
                             size_t offset, size_t sz)
        : data_(data), capacity_(cap), index_(index), offset_(offset), size_(sz) {}
    
template<typename T, typename Pointer, typename Reference, bool IsReverse>
template<typename U, typename P, typename R, bool IsRev>
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
circular_buffer_iterator(const circular_buffer_iterator<U, P, R, IsRev>& other)
        : data_(other.data_), capacity_(other.capacity_), 
          index_(other.index_), offset_(other.offset_), size_(other.size_) {}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::reference 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator*() const {
    size_t real_index = normalize();
    return data_[real_index];  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::pointer 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator->() const {
    size_t real_index = normalize();
    return &data_[real_index];  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator& 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator++() noexcept {
    if (IsReverse) --index_;  
    else ++index_;
    return *this;  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator++(int) noexcept{
    circular_buffer_iterator tmp = *this;
    ++*this; 
    return tmp;  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator& 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator--() noexcept {
    if (IsReverse) ++index_;  
    else --index_;
    return *this;   
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator--(int) noexcept {
    circular_buffer_iterator tmp = *this;
    --*this; 
    return tmp;  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator+(difference_type n) const noexcept {
    circular_buffer_iterator tmp = *this;
    if (IsReverse) tmp.index_ -= n;
    else tmp.index_ += n;
    return tmp;  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator-(difference_type n) const noexcept {
    circular_buffer_iterator tmp = *this;
    if (IsReverse) tmp.index_ += n;
    else tmp.index_ -= n;
    return tmp;  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator& 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator+=(difference_type n) noexcept {
    if (IsReverse) index_ -= n;
    else index_ += n;
    return *this;  
} 

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::circular_buffer_iterator& 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator-=(difference_type n) noexcept {
    if (IsReverse) index_ += n;
    else index_ -= n;
    return *this;  
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::difference_type 
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator-(const circular_buffer_iterator& other) const {
    return static_cast<difference_type>(index_ - other.index_); 
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
typename circular_buffer_iterator<T, Pointer, Reference, IsReverse>::reference
circular_buffer_iterator<T, Pointer, Reference, IsReverse>::operator[](difference_type n) const {
    if (IsReverse) return *(*this - n);
    else return *(*this + n);
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
bool circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
operator==(const circular_buffer_iterator& other) const noexcept{
    return data_ == other.data_ && index_ == other.index_;
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
bool circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
operator !=(const circular_buffer_iterator& other) const noexcept{
    return !(*this == other);
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
bool circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
operator<(const circular_buffer_iterator& other) const noexcept{
    return index_ < other.index_;
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
bool circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
operator<=(const circular_buffer_iterator& other) const noexcept{
    return index_ <= other.index_;
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
bool circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
operator>(const circular_buffer_iterator& other) const noexcept{
    return index_ > other.index_;
}

template<typename T, typename Pointer, typename Reference, bool IsReverse>
bool circular_buffer_iterator<T, Pointer, Reference, IsReverse>::
operator>=(const circular_buffer_iterator& other) const noexcept{
    return index_ >= other.index_;
}
