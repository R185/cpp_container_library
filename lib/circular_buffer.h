#pragma once    

#include <cstddef>    
#include <memory>     

template<typename T, typename Pointer, typename Reference, bool IsReverse>
class circular_buffer_iterator;

template<typename T, typename Allocator = std::allocator<T>>
class circular_buffer {
    public:

        using value_type             = T;
        using allocator_type         = Allocator;
        using alloc_traits           = std::allocator_traits<Allocator>;
        using size_type              = typename alloc_traits::size_type;
        using difference_type        = typename alloc_traits::difference_type;
        using reference              = value_type&;
        using const_reference        = const value_type&;
        using pointer                = typename alloc_traits::pointer;
        using const_pointer          = typename alloc_traits::const_pointer;
        using iterator               = circular_buffer_iterator<T, pointer, reference, false>;
        using reverse_iterator       = circular_buffer_iterator<T, pointer, reference, true>;
        using const_iterator         = circular_buffer_iterator<const T, const_pointer, const_reference, false>;
        using const_reverse_iterator = circular_buffer_iterator<const T, const_pointer, const_reference, true>;

        explicit circular_buffer(const Allocator& alloc = Allocator()); 
        explicit circular_buffer(size_type count, const Allocator& alloc = Allocator()); 
        circular_buffer(const circular_buffer& other); 
        circular_buffer(const circular_buffer& other, const Allocator& alloc); 

        circular_buffer(size_type count, const T& value, const Allocator& alloc = Allocator()); 
        
        template<class InputIterator> 
        circular_buffer(InputIterator first, InputIterator last, 
            size_type capacity, const Allocator& alloc = Allocator()); 

        circular_buffer(std::initializer_list<T> init,
        const Allocator& alloc = Allocator() ); 

        circular_buffer& operator=(const circular_buffer& other); 
        circular_buffer& operator=(circular_buffer&& other) noexcept; 

        ~circular_buffer(); 
        
        void swap(circular_buffer& other) noexcept; 

        void clear() noexcept; 
            
        allocator_type get_allocator() const noexcept; 
        
        reference front(); 
        reference back(); 
        reference operator[](size_type pos); 
        reference at(size_type pos); 

        const_reference front() const; 
        const_reference back() const; 
        const_reference operator[](size_type pos) const; 
        const_reference at(size_type pos) const; 
        
        iterator begin() noexcept; 
        const_iterator begin() const noexcept; 
        const_iterator cbegin() const noexcept; 
        
        iterator end() noexcept; 
        const_iterator end() const noexcept; 
        const_iterator cend() const noexcept; 

        reverse_iterator rbegin() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        
        reverse_iterator rend() noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crend() const noexcept; 
        
        bool empty() const noexcept; 
        size_type size() const noexcept; 
        size_type max_size() const noexcept; 
        size_type capacity() const noexcept; 
        
        void reserve(size_type new_capacity);  
        void assign(size_type count, const T& value); 
        
        template<class InputIt>
        void assign(InputIt first, InputIt last); 
        void assign(std::initializer_list<value_type> ilist); 

        void push_back(const value_type& value); 
        void pop_back(); 
        
        void push_front(const value_type& value); 
        void pop_front(); 

        
        iterator insert(const_iterator pos, const T& value);
        iterator insert(const_iterator pos, size_type count, const T& value); 
        template<class InputIt, typename = typename std::iterator_traits<InputIt>::iterator_category>
        iterator insert(const_iterator pos, InputIt first, InputIt last); 
        iterator insert(const_iterator pos, std::initializer_list<T> ilist);

        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last); 

        void resize(size_type count, const value_type& value = value_type());
         
        friend bool operator==(const circular_buffer& lhs, const circular_buffer& rhs){
            if (lhs.size_ != rhs.size_) return false;
            return std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }
        
        friend bool operator!=(const circular_buffer& lhs, const circular_buffer& rhs){
            return !(lhs == rhs);
        }

    private:
        
        Allocator alloc_;      
        pointer data_ = nullptr;         
        size_type capacity_ = 0;   
        size_type size_ = 0;       
        size_type head_ = 0;       
        
        size_type normalize_index(size_type index) const noexcept {
            return (index + head_) % capacity_;
        }

        void check_position(const_iterator pos) const;

        void move_elements_forward(size_type from, size_type count);
        void move_elements_backward(size_type from, size_type count);
};



template<typename T, typename Pointer, typename Reference, bool IsReverse>
class circular_buffer_iterator {
    public:
        
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Pointer;
        using reference         = Reference;

        circular_buffer_iterator() = default;
        circular_buffer_iterator(const circular_buffer_iterator&) = default;
        circular_buffer_iterator(pointer data, size_t capacity, 
                                size_t index, size_t offset, size_t size);
        
        template<typename U, typename P, typename R, bool Rev>
        circular_buffer_iterator(const circular_buffer_iterator<U, P, R, Rev>& other);
        
        reference operator*() const;
        pointer operator->() const;

        circular_buffer_iterator& operator++() noexcept;
        circular_buffer_iterator operator++(int) noexcept;
        circular_buffer_iterator& operator--() noexcept;
        circular_buffer_iterator operator--(int) noexcept;
        
        circular_buffer_iterator operator+(difference_type n) const noexcept;
        circular_buffer_iterator operator-(difference_type n) const noexcept;
        circular_buffer_iterator& operator+=(difference_type n) noexcept;
        circular_buffer_iterator& operator-=(difference_type n) noexcept;
        
        difference_type operator-(const circular_buffer_iterator& other) const;
        
        reference operator[](difference_type n) const;
        
        bool operator==(const circular_buffer_iterator& other) const noexcept;
        bool operator!=(const circular_buffer_iterator& other) const noexcept;
        bool operator<(const circular_buffer_iterator& other) const noexcept;
        bool operator<=(const circular_buffer_iterator& other) const noexcept;
        bool operator>(const circular_buffer_iterator& other) const noexcept;
        bool operator>=(const circular_buffer_iterator& other) const noexcept;
        
        friend circular_buffer_iterator operator+(difference_type n, 
                                                const circular_buffer_iterator& it) {it + n;}

    private:

        pointer data_;        
        size_t capacity_;    
        size_t index_;        
        size_t offset_;       
        size_t size_;
        
        size_t normalize() const noexcept {
            return (offset_ + index_) % capacity_;;
        }

        template<typename U, typename A>
        friend class circular_buffer;

        template<typename U, typename P, typename R, bool IsRev>
        friend class circular_buffer_iterator;
};


#include "circular_buffer.tpp"
