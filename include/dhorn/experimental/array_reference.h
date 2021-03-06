/*
 * Duncan Horn
 *
 * array_reference.h
 *
 * The array_reference type is meant to act a lot like std::vector, only with pre-allocated data. That is, it provides
 * bounds checking, iterating, etc. Its functionality also diverges a bit by acting more like a pointer than a vector
 * (e.g. you can increment, etc.). Note that while incrementing can do a bounds check, decrement cannot (since the
 * array_reference may have been initially created in the middle of the array)
 */
#pragma once

#include <cassert>
#include <iterator>

#include "../iterator.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * array_reference
         */
        template <typename Ty>
        class array_reference
        {
        public:
            /*
             * Types
             */
            using value_type = Ty;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = Ty &;
            using const_reference = const Ty &;
            using pointer = Ty *;
            using const_pointer = const Ty *;
            using iterator = array_iterator<array_reference>;
            using const_iterator = const_array_iterator<array_reference>;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;



            /*
             * Constructor(s)/Destructor
             */
            array_reference(void) :
                _ptr(nullptr),
                _size(0)
            {
            }

            array_reference(pointer ptr, size_type size) :
                _ptr(ptr),
                _size(size)
            {
            }

            template <std::size_t Size>
            array_reference(Ty(&arr)[Size]) :
                _ptr(arr),
                _size(Size)
            {
            }

            // Default copy/move



            /*
             * Operators
             */
            reference operator[](size_type pos)
            {
                assert(pos < this->_size);
                return this->_ptr[pos];
            }

            const_reference operator[](size_type pos) const
            {
                assert(pos < this->_size);
                return this->_ptr[pos];
            }

            reference operator*(void)
            {
                assert(this->_size);
                return *this->_ptr;
            }

            const_reference operator*(void) const
            {
                assert(this->_size);
                return *this->_ptr;
            }

            array_reference &operator+=(difference_type delta)
            {
                assert((delta < 0) || (static_cast<std::size_t>(delta) <= this->_size));
                this->_ptr += delta;
                this->_size -= delta;

                return *this;
            }

            array_reference &operator-=(difference_type delta)
            {
                assert((delta > 0) || (static_cast<std::size_t>(-delta) <= this->_size));
                this->_ptr -= delta;
                this->_size += delta;

                return *this;
            }

            array_reference &operator++(void)
            {
                return *this += 1;
            }

            array_reference operator++(int /*unused*/)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }

            array_reference &operator--(void)
            {
                return *this -= 1;
            }

            array_reference operator--(int /*unused*/)
            {
                auto copy = *this;
                --(*this);
                return copy;
            }

            array_reference operator+(difference_type delta) const
            {
                auto copy = *this;
                copy += delta;
                return copy;
            }

            array_reference operator-(difference_type delta) const
            {
                auto copy = *this;
                copy -= delta;
                return copy;
            }



            /*
             * Functions
             */
            reference at(size_type pos)
            {
                if (pos >= this->_size)
                {
                    throw std::out_of_range("array_reference index out of range");
                }

                return this->_ptr[pos];
            }

            const_reference at(size_type pos) const
            {
                if (pos >= this->_size)
                {
                    throw std::out_of_range("array_reference index out of range");
                }

                return this->_ptr[pos];
            }

            reference front(void)
            {
                return (*this)[0];
            }

            const_reference front(void) const
            {
                return (*this)[0];
            }

            reference back(void)
            {
                return (*this)[this->_size - 1];
            }

            const_reference back(void) const
            {
                return (*this)[this->_size - 1];
            }

            pointer data(void)
            {
                return this->_ptr;
            }

            const_pointer data(void) const
            {
                return this->_ptr;
            }

            bool empty(void) const noexcept
            {
                return this->_size == 0;
            }

            size_type size(void) const noexcept
            {
                return this->_size;
            }

            size_type max_size(void) const noexcept
            {
                return std::numeric_limits<size_type>::max();
            }



            /*
             * Resizing/casting
             */
            void resize(size_type size)
            {
                // It's impossible to check the safety of this operation since it is, by definition, already unsafe
                this->_size = size;
            }

            template <typename TargetTy>
            array_reference<TargetTy> cast_to(void) const
            {
                // Convert our size to units of bytes. This has potential overflow, but if that's the case, then the array
                // is not valid (it, by definition, cannot hold all the values it says it holds). If anyone tries to do
                // that, they will get undefined behavior when calling this function
                auto sizeBytes = this->_size * sizeof(Ty);

                // This will round down (if rounding is necessary), which is what we want
                return array_reference<TargetTy>(reinterpret_cast<TargetTy *>(this->_ptr), sizeBytes / sizeof(TargetTy));
            }



            /*
             * Iterators
             */
            iterator begin(void)
            {
                return iterator(this->_ptr);
            }

            const_iterator begin(void) const
            {
                return const_iterator(this->_ptr);
            }

            const_iterator cbegin(void) const
            {
                return const_iterator(this->_ptr);
            }

            iterator end(void)
            {
                return iterator(this->_ptr + this->_size);
            }

            const_iterator end(void) const
            {
                return const_iterator(this->_ptr + this->_size);
            }

            const_iterator cend(void) const
            {
                return const_iterator(this->_ptr + this->_size);
            }

            reverse_iterator rbegin(void)
            {
                return reverse_iterator(end());
            }

            const_reverse_iterator rbegin(void) const
            {
                return const_reverse_iterator(cend());
            }

            const_reverse_iterator crbegin(void) const
            {
                return const_reverse_iterator(cend());
            }

            reverse_iterator rend(void)
            {
                return reverse_iterator(begin());
            }

            const_reverse_iterator rend(void) const
            {
                return const_reverse_iterator(cbegin());
            }

            const_reverse_iterator crend(void) const
            {
                return const_reverse_iterator(cbegin());
            }



        private:

            pointer _ptr;
            size_type _size;
        };



        /*
         * Operators
         */
#pragma region Operators

        template <
            typename LhsTy,
            typename RhsTy,
            typename = std::enable_if_t<std::is_same_v<const LhsTy, const RhsTy>>>
        inline bool operator==(const array_reference<LhsTy> &lhs, const array_reference<RhsTy> &rhs)
        {
            if (lhs.data() == rhs.data() && lhs.size() == rhs.size())
            {
                return true;
            }
            else if (lhs.size() != rhs.size())
            {
                return false;
            }

            return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        }

        template <
            typename LhsTy,
            typename RhsTy,
            typename = std::enable_if_t<std::is_same_v<const LhsTy, const RhsTy>>>
        inline bool operator!=(const array_reference<LhsTy> &lhs, const array_reference<RhsTy> &rhs)
        {
            return !(lhs == rhs);
        }

        template <
            typename LhsTy,
            typename RhsTy,
            typename = std::enable_if_t<std::is_same_v<const LhsTy, const RhsTy>>>
        inline bool operator<(const array_reference<LhsTy> &lhs, const array_reference<RhsTy> &rhs)
        {
            return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                std::less<const LhsTy>());
        }

        template <
            typename LhsTy,
            typename RhsTy,
            typename = std::enable_if_t<std::is_same_v<const LhsTy, const RhsTy>>>
        inline bool operator>(const array_reference<LhsTy> &lhs, const array_reference<RhsTy> &rhs)
        {
            return std::lexicographical_compare(
                rhs.begin(), rhs.end(),
                lhs.begin(), lhs.end(),
                std::less<const LhsTy>());
        }

        template <
            typename LhsTy,
            typename RhsTy,
            typename = std::enable_if_t<std::is_same_v<const LhsTy, const RhsTy>>>
        inline bool operator<=(const array_reference<LhsTy> &lhs, const array_reference<RhsTy> &rhs)
        {
            return !(lhs > rhs);
        }

        template <
            typename LhsTy,
            typename RhsTy,
            typename = std::enable_if_t<std::is_same_v<const LhsTy, const RhsTy>>>
        inline bool operator>=(const array_reference<LhsTy> &lhs, const array_reference<RhsTy> &rhs)
        {
            return !(lhs < rhs);
        }

#pragma endregion
    }
}
