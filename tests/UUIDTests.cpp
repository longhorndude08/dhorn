/*
 * Duncan Horn
 *
 * UUIDTests.cpp
 *
 * Tests for dhorn::experimental::uuid structure
 */
#include "stdafx.h"

#include <dhorn/experimental/uuid.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


static const std::uint8_t byte_array[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                           0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
static const std::uint32_t int_array[] = { 0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF };

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(UUIDTests)
        {
            TEST_METHOD(DefaultConstructorTest)
            {
                dhorn::experimental::uuid id;

                // Make sure all fields are zero
                for (std::size_t i = 0; i < 16; i++)
                {
                    ASSERT_TRUE(id.data[i] == 0);
                }
                for (std::size_t i = 0; i < 4; i++)
                {
                    ASSERT_TRUE(id.data32[i] == 0);
                }
            }

            TEST_METHOD(ByteArrayConstructorAssignmentTest)
            {
                // Cannot construct with non-16-size std::uint8_t array
                std::uint8_t invalid_array[] = { 1, 2, 3, 4 };
                (void)invalid_array;
                ASSERT_FALSE(std::is_constructible_v<dhorn::experimental::uuid, decltype(invalid_array)>);

                // Should be able to contruct with std::uint8_t[16]
                dhorn::experimental::uuid id(byte_array);

                for (std::size_t i = 0; i < 16; i++)
                {
                    ASSERT_TRUE(id.data[i] == byte_array[i]);
                }

                // Should be able to use initializer list syntax
                id = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

                for (std::size_t i = 0; i < 16; i++)
                {
                    ASSERT_TRUE(id.data[i] == byte_array[i]);
                }
            }

            TEST_METHOD(IntegerArrayConstructorAssignmentTest)
            {
                // Cannot construct with non-16-size std::uint8_t array
                std::uint32_t invalid_array[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
                (void)invalid_array;
                ASSERT_FALSE(std::is_constructible_v<dhorn::experimental::uuid, decltype(invalid_array)>);

                // Should be able to contruct with std::uint8_t[16]
                dhorn::experimental::uuid id(int_array);

                for (std::size_t i = 0; i < 4; i++)
                {
                    ASSERT_TRUE(id.data32[i] == int_array[i]);
                }

                // Should be able to use initializer list syntax
                id = { 0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF };

                for (std::size_t i = 0; i < 4; i++)
                {
                    ASSERT_TRUE(id.data32[i] == int_array[i]);
                }
            }

            TEST_METHOD(GuidConstructorAssignmentTest)
            {
                // Assuming little endian, so the non-8-bit integers' bytes are reversed
                dhorn::experimental::uuid id = { 0x33221100, 0x5544, 0x7766,
                    { 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF } };

                for (std::size_t i = 0; i < 16; i++)
                {
                    ASSERT_TRUE(id.data[i] == byte_array[i]);
                }

                // Construct with GUID
                static const GUID guid = { 0x40ea2f58, 0x4589, 0x4ae6,
                    { 0x8f, 0xfa, 0xf1, 0xbc, 0x36, 0xcd, 0x98, 0x4c } };
                id = guid;

                ASSERT_TRUE(guid.Data1 == id.Data1);
                ASSERT_TRUE(guid.Data2 == id.Data2);
                ASSERT_TRUE(guid.Data3 == id.Data3);
                for (std::size_t i = 0; i < 8; i++)
                {
                    ASSERT_TRUE(guid.Data4[i] == id.Data4[i]);
                }
            }

            TEST_METHOD(CopyConstructorTest)
            {
                dhorn::experimental::uuid id(byte_array);
                dhorn::experimental::uuid id2 = id;

                for (std::size_t i = 0; i < 16; i++)
                {
                    ASSERT_TRUE(id2.data[i] == byte_array[i]);
                }
            }

            TEST_METHOD(ComparisonTest)
            {
                dhorn::experimental::uuid x = { 0xa160dd94, 0x3c9d, 0x49cf,
                    { 0xaf, 0x65, 0xf7, 0xa6, 0x59, 0x23, 0x25, 0xfd } };
                dhorn::experimental::uuid y = { 0xd51675dc, 0xdebc, 0x49bc,
                    { 0x91, 0xa9, 0xf4, 0xa2, 0x85, 0x95, 0x7e, 0x6e } };

                ASSERT_TRUE(x == x);
                ASSERT_TRUE(x != y);

                ASSERT_FALSE(x < x);
                ASSERT_TRUE(x < y);
                ASSERT_TRUE(x <= x);
                ASSERT_TRUE(x <= y);

                ASSERT_FALSE(x > x);
                ASSERT_TRUE(y > x);
                ASSERT_TRUE(x >= x);
                ASSERT_TRUE(y >= x);
            }

            TEST_METHOD(HashTest)
            {
                dhorn::experimental::uuid x = { 0xa160dd94, 0x3c9d, 0x49cf,
                { 0xaf, 0x65, 0xf7, 0xa6, 0x59, 0x23, 0x25, 0xfd } };
                dhorn::experimental::uuid y = { 0xd51675dc, 0xdebc, 0x49bc,
                { 0x91, 0xa9, 0xf4, 0xa2, 0x85, 0x95, 0x7e, 0x6e } };

                std::hash<dhorn::experimental::uuid> hasher;
                ASSERT_TRUE(hasher(x) != hasher(y));
            }

#ifdef _DEBUG
            TEST_METHOD(DestructorTest)
            {
                dhorn::experimental::uuid id;

                // Should be zero-initialized. Once we run the destructor, it should be filled with
                // the patern 0xCC...
                id.~uuid();

                for (std::size_t i = 0; i < 16; i++)
                {
                    ASSERT_TRUE(id.data[i] == 0xCC);
                }
                for (std::size_t i = 0; i < 4; i++)
                {
                    ASSERT_TRUE(id.data32[i] == 0xCCCCCCCC);
                }
            }
#endif
        };
    }
}
