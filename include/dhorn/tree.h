/*
 * tree.h
 *
 * Duncan Horn
 *
 *
 */
#pragma once

#ifdef  WIN32
#include <sal.h>
#endif  /* WIN32 */

#include <algorithm>
#include <vector>

namespace dhorn
{
    /*
     * Tree iterator types. These types should never be referenced directly. 
     */
#pragma region Iterators

    template <typename _Tree>
    class _dhorn_tree_const_iterator :
        public std::iterator<std::bidirectional_iterator_tag, typename _Tree::value_type>
    {
    public:
        using value_type = typename _Tree::value_type;
        using difference_type = typename _Tree::allocator_type::difference_type;
        using pointer = typename _Tree::const_pointer;
        using reference = typename _Tree::const_reference;
    };

    template <typename _Tree>
    class _dhorn_tree_iterator :
        public _dhorn_tree_const_iterator<_Tree>
    {
    public:
        using pointer = typename _Tree::pointer;
        using reference = typename _Tree::reference;

    };

#pragma endregion



    /*
     * Tree node class. This is an internal class and should never be referenced directly by client
     * code as it may change/disappear without notice.
     */
#pragma region Tree Node

    template <typename _Tree>
    class _dhorn_tree_node
    {
    public:
        using value_type = typename _Tree::value_type;
        using size_type = typename _Tree::size_type;
        using node_pointer = _dhorn_tree_node *;



        /*
         * Constructor(s)/Destructor
         */
        _dhorn_tree_node(void) :
            _size(0),
            _front(nullptr),
            _back(nullptr)
        {
        }

        _dhorn_tree_node(_In_ const _dhorn_tree_node &other) :
            _size(other._size),
            _value(other._value)
        {
            // TODO
        }

        _dhorn_tree_node(_Inout_ _dhorn_tree_node &&other) :
            _size(other._size),
            _front(other._front),
            _back(other._back),
            _value(std::move(other._value))
        {
            other._size = 0;
            other._front = nullptr;
            other._back = nullptr;
        }

        _dhorn_tree_node(_In_ const value_type &value) :
            _size(0),
            _front(nullptr),
            _back(nullptr),
            _value(value)
        {
        }

        _dhorn_tree_node(_Inout_ value_type &&value) :
            _size(0),
            _front(nullptr),
            _back(nullptr),
            _value(std::move(value))
        {
        }

        ~_dhorn_tree_node(void)
        {
            // TODO
        }



        /*
         * Assignment Operators
         */
        _dhorn_tree_node &operator=(_In_ const _dhorn_tree_node &other)
        {

        }

        _dhorn_tree_node &operator=(_In_ _dhorn_tree_node &&other)
        {

        }



        /*
         * Accessors
         */
        value_type &value(void) _NOEXCEPT
        {
            return this->_value;
        }

        const value_type &value(void) const _NOEXCEPT
        {
            return this->_value;
        }



        /*
         * Capacity
         */
        size_type size(void) const _NOEXCEPT
        {
            return this->_size;
        }



    private:

        value_type   _value;
        size_type    _size;
        node_pointer _front;
        node_pointer _back;
    };

#pragma endregion



    /*
     * dhorn::tree
     */
#pragma region tree

    template <typename _Ty, typename _Alloc = std::allocator<_Ty>>
    class tree
    {
    public:

        /*
         * Type/Value Definitions
         */
        using _MyType = tree<_Ty, _Alloc>;
        using value_type = _Ty;
        using allocator_type = _Alloc;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using iterator = _dhorn_tree_iterator<_MyType>;
        using const_iterator = _dhorn_tree_const_iterator<_MyType>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using difference_type = typename std::iterator_traits<iterator>::difference_type;
        using size_type = typename allocator_type::size_type;



        /*
         * Constructor(s)/Destructor
         */
        tree(void)
        {
            // TODO
        }

        tree(_In_ const tree &other)
        {
            // TODO
        }

        tree(_Inout_ tree &&other)
        {
            // TODO
        }



        /*
         * Assignment Operators
         */
        tree &operator=(_In_ const tree &other)
        {
            if (this != &other)
            {
                // TODO
            }

            return *this;
        }

        tree &operator=(_Inout_ tree &&other)
        {
            // TODO

            return *this;
        }



        /*
         * Iterators
         */
        iterator begin(void) _NOEXCEPT
        {
            // TODO
        }

        const_iterator begin(void) const _NOEXCEPT
        {
            // TODO
        }

        iterator end(void) _NOEXCEPT
        {
            // TODO
        }

        const_iterator end(void) const _NOEXCEPT
        {
            // TODO
        }

        reverse_iterator rbegin(void) _NOEXCEPT
        {
            return reverse_iterator(this->end());
        }

        const_reverse_iterator rbegin(void) const _NOEXCEPT
        {
            return const_reverse_iterator(this->end());
        }

        reverse_iterator rend(void) _NOEXCEPT
        {
            return reverse_iterator(this->begin());
        }

        const_reverse_iterator rend(void) const _NOEXCEPT
        {
            return const_reverse_iterator(this->begin());
        }

        const_iterator cbegin(void) const _NOEXCEPT
        {
            return this->begin();
        }

        const_iterator cend(void) const _NOEXCEPT
        {
            return this->end();
        }

        const_reverse_iterator crbegin(void) const _NOEXCEPT
        {
            return this->rbegin();
        }

        const_reverse_iterator crend(void) const _NOEXCEPT
        {
            return this->rend();
        }



        /*
         * Capacity
         */
        size_type size(void) const _NOEXCEPT
        {
            return this->_size;
        }

        size_type max_size(void) const _NOEXCEPT
        {
            return this->_alloc.max_size();
        }

        bool empty(void) const _NOEXCEPT
        {
            return this->_size == 0;
        }



        /*
         * Modifiers
         */
        iterator insert(_In_ const_iterator parent, _In_ const value_type &val)
        {
            // TODO
        }

        iterator insert(_In_ const_iterator parent, _Inout_ value_type &&val)
        {
            // TODO
        }

        template <typename _InputIterator>
        void insert(_In_ const_iterator parent, _In_ _InputIterator first, _In_ _InputIterator last)
        {
            std::for_each(first, last, [&](_In_ const value_type &val)
            {
                // TODO
            });
        }

        void insert(_In_ const_iterator parent, _In_ std::initializer_list<value_type> list)
        {
            for (auto &val : list)
            {
                // TODO
            }
        }

        iterator erase(_In_ const_iterator position)
        {
            // TODO
        }

        iterator erase(_In_ const_iterator first, _In_ const_iterator last)
        {
            // TODO
        }

        void swap(_Inout_ tree &other)
        {
            // TODO
        }

        void clear(void) _NOEXCEPT
        {
            // TODO

            this->_size = 0;
        }

        template <typename... _Args>
        iterator emplace(_In_ const_iterator parent, _In_ _Args&&... args)
        {
            // TODO
        }



        /*
         * Allocator
         */
        allocator_type get_allocator(void) const _NOEXCEPT
        {
            return this->_alloc;
        }



    private:

        allocator_type  _alloc;
        size_type       _size;
    };

#pragma endregion
}



#if     (!defined _DHORN_NO_STD_SWAP) && (!defined _DHORN_NO_STD)

namespace std
{
    template <typename _Ty, typename _Alloc>
    inline void swap(_Inout_ dhorn::tree<_Ty, _Alloc> &lhs, _Inout_ dhorn::tree<_Ty, _Alloc> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif  /* (!defined _DHORN_NO_STD_SWAP) && (!defined _DHORN_NO_STD) */