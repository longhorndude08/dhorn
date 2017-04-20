/*
 * Duncan Horn
 *
 * UniqueTests.cpp
 *
 * Tests for the dhorn unique class
 */
#include "stdafx.h"

#include <dhorn/unique.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(UniqueTests)
    {
#pragma region Test Types

        struct base
        {
        };

        struct derived :
            public base
        {
        };

#pragma endregion



#pragma region Test Traits

        // Traits type that is guaranteed to be empty. Useful for size tests...
        template <typename Ty>
        struct empty_traits
        {
            // Decay arrays to pointers
            using value_type = std::decay_t<Ty>;

            static constexpr bool is_valid(const value_type&) { return true; }
            static constexpr value_type default_value() { return value_type{}; }

            constexpr void operator()(const value_type&) {}
        };

        template <typename Ty>
        using unique_empty = unique<Ty, empty_traits<Ty>>;

        class empty_int_traits : public empty_traits<int> {};
        using unique_empty_int = unique<int, empty_int_traits>;



        template <typename Ty>
        struct non_empty_traits
        {
            // Decay arrays to pointers
            using value_type = std::decay_t<Ty>;

            static constexpr bool is_valid(const value_type&) { return true; }
            static constexpr value_type default_value() { return value_type{}; }

            constexpr void operator()(const value_type&) {}

            // Hold a `Ty` as the member. This makes our math easier (multiply by two) and eliminates our need to worry
            // about padding
            value_type value;
        };

        template <typename Ty>
        using unique_non_empty = unique<Ty, non_empty_traits<Ty>>;



        template <typename CharT>
        struct unique_basic_string_traits
        {
            using value_type = std::basic_string<CharT>;

            static constexpr bool valid(const value_type& value) noexcept
            {
                return !value.empty();
            }

            static constexpr value_type default_value() noexcept
            {
                return value_type{};
            }

            static void destroy(value_type&) noexcept
            {
                // Strings delete themselves
            }
        };

        template <typename CharT>
        using unique_basic_string = unique < std::basic_string<CharT>, unique_basic_string_traits<CharT>>;



        template <typename Ty, size_t Size, Ty Value = Ty{}>
        struct clear_buffer_unique_traits
        {
            using value_type = Ty*;

            static constexpr bool valid(value_type)
            {
                // Should never be given a null value, so always valid
                return true;
            }

            [[noreturn]] static constexpr value_type default_value()
            {
                Assert::Fail("Did not expect default construction");
            }

            static void destroy(value_type value)
            {
                for (size_t i = 0; i < Size; ++i)
                {
                    value[i] = Value;
                }
            }
        };

        template <typename Ty, size_t Size, Ty Value = Ty{}>
        using unique_buffer = unique<Ty, clear_buffer_unique_traits<Ty, Size, Value>>;

#pragma endregion



#pragma region Size Tests

        TEST_METHOD(EmptyTraitsSizeTest)
        {
            Assert::AreEqual(sizeof(int), sizeof(unique_empty<int>));
            Assert::AreEqual(sizeof(std::string), sizeof(unique_empty<std::string>));

            Assert::AreEqual(sizeof(int*), sizeof(unique_empty<int*>));
            Assert::AreEqual(sizeof(int*), sizeof(unique_empty<int[]>));
        }

        TEST_METHOD(NonEmptyTraitsSizeTest)
        {
            Assert::AreEqual(sizeof(int) * 2, sizeof(unique_non_empty<int>));
            Assert::AreEqual(sizeof(std::string) * 2, sizeof(unique_non_empty<std::string>));

            Assert::AreEqual(sizeof(int*) * 2, sizeof(unique_non_empty<int*>));
            Assert::AreEqual(sizeof(int*) * 2, sizeof(unique_non_empty<int[]>));
        }

        TEST_METHOD(UniquePtrSizeTest)
        {
            Assert::AreEqual(sizeof(void*), sizeof(unique_ptr<int>));
            Assert::AreEqual(sizeof(void*), sizeof(unique_ptr<std::string>));

            Assert::AreEqual(sizeof(void*), sizeof(unique_ptr<int[]>));
        }

#pragma endregion



#pragma region Traits Tests

        template <typename Ty>
        struct has_value_type_traits
        {
            using value_type = std::unique_ptr<Ty>;

            static constexpr bool is_valid(const value_type& value) { return value; }
            static constexpr value_type default_value() { return {}; }
            constexpr void operator()(const value_type&) {}
        };

        template <typename Ty>
        struct no_value_type_traits
        {
            static constexpr bool is_valid(const Ty&) { return true; }
            static constexpr Ty default_value() { return {}; }
            constexpr void operator()(const Ty&) {}
        };

        TEST_METHOD(ValueTypeAliasTest)
        {
            Assert::IsTrue(std::is_same_v<int, unique<int, no_value_type_traits<int>>::value_type>);
            Assert::IsTrue(std::is_same_v<std::unique_ptr<int>, unique<int, has_value_type_traits<int>>::value_type>);

            Assert::IsTrue(std::is_same_v<int*, unique<int*>::value_type>);
            Assert::IsTrue(std::is_same_v<int*, unique<int(*)[]>::value_type>);

            Assert::IsTrue(std::is_same_v<int*, unique_ptr<int>::value_type>);
            Assert::IsTrue(std::is_same_v<int*, unique_ptr<int[]>::value_type>);
        }

#pragma endregion



#pragma region Tests

#pragma region Constructor Tests

        TEST_METHOD(DefaultConstructionTest)
        {
            Assert::IsTrue(std::is_default_constructible_v<unique_empty<int>>);
            Assert::IsTrue(std::is_default_constructible_v<unique_ptr<int>>);
            Assert::IsTrue(std::is_default_constructible_v<unique_ptr<int[]>>);

            // We can't test with std::is_default_constructible_v since SFINAE is not involved (it only fails when
            // trying to instantiate the function template), but the following should fail to compile:
            //unique<int, empty_traits<int>&> x;
            //unique<int, empty_traits<int>*> y;

            unique_empty<int> uniqueInt;
            unique_ptr<int> intPtr;
            unique_ptr<int[]> intArray;
        }

        TEST_METHOD(ValueConstructionTest)
        {
            // Constructing a unique<int, ...> should follow int's construction rules
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, const int>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, char>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<int>, int*>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<int>, std::string>);

            unique_empty<int> uniqueInt(8);

            // Constructing a unique<std::string, ...> should follow std::string's construction rules
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, char*>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, const char*>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, const std::string&>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<std::string>, std::string&&>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<std::string>, const wchar_t*>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<std::string>, const std::wstring&>);
            Assert::IsFalse(std::is_constructible_v<unique_empty<std::string>, std::wstring&&>);

            unique_empty<std::string> uniqueString("foo");

            // Constructing a unique_ptr should follow the same construction rules as std::unique_ptr
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, std::nullptr_t>);

            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int>, int* const>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int>, const int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int>, int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int>, const int*>);

            Assert::IsTrue(std::is_constructible_v<unique_ptr<base>, derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base>, const derived*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base>, derived*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base>, const derived*>);

            Assert::IsFalse(std::is_constructible_v<unique_ptr<derived>, base*>);

            unique_ptr<int> uniqueIntPtr(nullptr);
            unique_ptr<int> uniqueIntPtr2(new int());
            unique_ptr<base> uniqueBasePtr(new derived());

            // Constructing a unique_ptr<T[]> should follow the same construction rules as std::unique_ptr<T[]>
            Assert::IsTrue(std::is_constructible_v<unique_ptr<int[]>, std::nullptr_t>);

            Assert::IsTrue(std::is_constructible_v<unique_ptr<int[]>, int*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<int[]>, const int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int[]>, int*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const int[]>, const int*>);

            Assert::IsTrue(std::is_constructible_v<unique_ptr<base[]>, base*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, const base*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base[]>, base*>);
            Assert::IsTrue(std::is_constructible_v<unique_ptr<const base[]>, const base*>);

            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<base[]>, const derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<const base[]>, derived*>);
            Assert::IsFalse(std::is_constructible_v<unique_ptr<const base[]>, const derived*>);

            Assert::IsFalse(std::is_constructible_v<unique_ptr<derived[]>, base*>);

            unique_ptr<int[]> uniqueIntArray(nullptr);
            unique_ptr<int[]> uniqueIntArray2(new int[4]);
            unique_ptr<base[]> uniqueBaseArray(new base[8]);
        }

        TEST_METHOD(ConstructWithTraitsTest)
        {
            empty_traits<int> emptyIntTraits;

            // Value
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, empty_traits<int>>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, empty_traits<int>&>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, const empty_traits<int>&>);
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, int, empty_traits<int>&&>);

            unique_empty<int> uniqueIntValue1(0, emptyIntTraits);
            unique_empty<int> uniqueIntValue2(0, empty_traits<int>{});

            // Reference
            Assert::IsFalse(std::is_constructible_v<unique<int, empty_traits<int>&>, int, empty_traits<int>>);
            Assert::IsTrue(std::is_constructible_v<unique<int, empty_traits<int>&>, int, empty_traits<int>&>);
            Assert::IsFalse(std::is_constructible_v<unique<int, empty_traits<int>&>, int, const empty_traits<int>&>);
            Assert::IsFalse(std::is_constructible_v<unique<int, empty_traits<int>&>, int, empty_traits<int>&&>);

            unique<int, empty_traits<int>&> uniqueIntValue3(8, emptyIntTraits);

            std::unique_ptr<int> p;
        }

        TEST_METHOD(MoveConstructionTest)
        {
            // Value
            Assert::IsTrue(std::is_move_constructible_v<unique_empty<int>>);

            unique_empty<int> uniqueInt(8);
            unique_empty<int> uniqueIntCopy(std::move(uniqueInt));
            Assert::AreEqual(uniqueInt.get(), 0);
            Assert::AreEqual(uniqueIntCopy.get(), 8);

            // Reference Value
            Assert::IsTrue(std::is_move_constructible_v<unique_empty<int&>>);
            // References would assign through, so testing move construction is kind of pointless... In fact, it
            // probably wouldn't even compile since default_value wouldn't make sense

            // Reference Traits
            Assert::IsTrue(std::is_move_constructible_v<unique<int, unique_empty<int>&>>);

            empty_traits<int> traits;
            unique<int, empty_traits<int>&> uniqueIntRef(8, traits);
            unique<int, empty_traits<int>&> uniqueIntRefCopy(std::move(uniqueIntRef));
            Assert::AreEqual(uniqueIntRef.get(), 0);
            Assert::AreEqual(uniqueIntRefCopy.get(), 8);
        }

        TEST_METHOD(MoveConversionConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<unique_empty<int>, unique_empty_int&&>);
            Assert::IsFalse(std::is_constructible_v<unique_empty_int, unique_empty<int>&&>);

            unique_empty_int from(8);
            unique_empty<int> to(std::move(from));
            Assert::AreEqual(0, from.get());
            Assert::AreEqual(8, to.get());
        }

        TEST_METHOD(CannotCopyTest)
        {
            Assert::IsFalse(std::is_copy_constructible_v<unique_empty<int>>);
            Assert::IsFalse(std::is_copy_constructible_v<unique_ptr<int>>);
            Assert::IsFalse(std::is_copy_constructible_v<unique_ptr<int[]>>);
        }

#pragma endregion

#pragma endregion
    };
}
