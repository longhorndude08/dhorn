/*
 * Duncan Horn
 *
 * UnicodeIteratorTests.cpp
 *
 * Tests for the unicode/iterator.h functions and types
 */

#include <array>
#include <dhorn/unicode/iterator.h>
#include <forward_list>
#include <gtest/gtest.h>
#include <list>

const char empty_string8[] = u8"";
const char foobar_string8[] = u8"foobar";
const char test_string8[] = u8"\u007F\u0080\u07FF\u0800\uD7FF\uE000\uFFFF\U00010000\U0010FFFF";

const char16_t empty_string16[] = u"";
const char16_t foobar_string16[] = u"foobar";
const char16_t test_string16[] = u"\u007F\u0080\u07FF\u0800\uD7FF\uE000\uFFFF\U00010000\U0010FFFF";
const char16_t foobar_string16_be[] = u"\x6600\x6f00\x6f00\x6200\x6100\x7200";
const char16_t test_string16_be[] = u"\x7F00\x8000\xFF07\x0008\xFFD7\x00E0\xFFFF\x00D8\x00DC\xFFDB\xFFDF";

const char32_t empty_string32[] = U"";
const char32_t foobar_string32[] = U"foobar";
const char32_t test_string32[] = U"\u007F\u0080\u07FF\u0800\uD7FF\uE000\uFFFF\U00010000\U0010FFFF";
const char32_t foobar_string32_be[] = U"\x66000000\x6f000000\x6f000000\x62000000\x61000000\x72000000";
const char32_t test_string32_be[] = U"\x7F000000\x80000000\xFF070000\x00080000\xFFD70000\x00E00000\xFFFF0000\x00000100\xFFFF1000";

template <typename Ty, std::size_t Size>
auto copy_array(const Ty (&array)[Size])
{
    std::array<Ty, Size> result;
    std::copy(std::begin(array), std::end(array), std::begin(result));
    return result;
}

#define DUPLICATE_STRING(Prefix, String)    Prefix ## String, U ## String

TEST(UnicodeIteratorTests, IteratorCategoryTest)
{
    // c-style strings are random access and should therefore be bidirectional
    ASSERT_TRUE((std::is_same_v<
        std::bidirectional_iterator_tag,
        dhorn::unicode::iterator<const char*>::iterator_category>));

    // std::string is random access too
    ASSERT_TRUE((std::is_same_v<
        std::bidirectional_iterator_tag,
        dhorn::unicode::iterator<std::string::iterator>::iterator_category>));

    // std::list is bidirectional, meaning that utf_iterator should match it
    ASSERT_TRUE((std::is_same_v<
        std::bidirectional_iterator_tag,
        dhorn::unicode::iterator<std::list<char>::iterator>::iterator_category>));

    // std::forward_list is forward only, meaning utf_iterator should cap at forward
    ASSERT_TRUE((std::is_same_v<
        std::forward_iterator_tag,
        dhorn::unicode::iterator<std::forward_list<char>::iterator>::iterator_category>));
}

template <
    typename Itr,
    dhorn::unicode::encoding Encoding = dhorn::unicode::character_encoding_v<typename std::iterator_traits<Itr>::value_type>>
void DoPreIncrementTest(Itr begin, const char32_t* expected)
{
    dhorn::unicode::iterator<Itr, Encoding> itr(begin);

    for (std::size_t i = 0; expected[i]; ++i)
    {
        // Copies should be okay
        auto copy = itr;
        ASSERT_EQ(expected[i], *copy);
        ASSERT_EQ(expected[i + 1], *++copy);

        ASSERT_EQ(expected[i], *itr);
        ASSERT_EQ(expected[i + 1], *++itr);
    }

    ASSERT_EQ(U'\0', *itr);
}

template <dhorn::unicode::encoding Encoding, typename Itr>
void DoPreIncrementTest(Itr begin, const char32_t* expected)
{
    DoPreIncrementTest<Itr, Encoding>(begin, expected);
}

template <
    typename Itr,
    dhorn::unicode::encoding Encoding = dhorn::unicode::character_encoding_v<typename std::iterator_traits<Itr>::value_type>>
void DoPostIncrementTest(Itr begin, const char32_t* expected)
{
    dhorn::unicode::iterator<Itr, Encoding> itr(begin);

    for (std::size_t i = 0; expected[i]; ++i)
    {
        // Copies should be okay
        auto copy = itr;
        ASSERT_EQ(expected[i], *copy);
        ASSERT_EQ(expected[i], *copy++);

        ASSERT_EQ(expected[i], *itr);
        ASSERT_EQ(expected[i], *itr++);
    }

    ASSERT_EQ(U'\0', *itr);
}

template <dhorn::unicode::encoding Encoding, typename Itr>
void DoPostIncrementTest(Itr begin, const char32_t* expected)
{
    DoPostIncrementTest<Itr, Encoding>(begin, expected);
}

TEST(UnicodeIteratorTests, Utf8PreIncrementTest)
{
    DoPreIncrementTest(empty_string8, empty_string32);
    DoPreIncrementTest(foobar_string8, foobar_string32);
    DoPreIncrementTest(test_string8, test_string32);

    std::forward_list<char> list(std::begin(test_string8), std::end(test_string8));
    DoPreIncrementTest(list.begin(), test_string32);
}

TEST(UnicodeIteratorTests, Utf8PostIncrementTest)
{
    DoPostIncrementTest(empty_string8, empty_string32);
    DoPostIncrementTest(foobar_string8, foobar_string32);
    DoPostIncrementTest(test_string8, test_string32);

    std::forward_list<char> list(std::begin(test_string8), std::end(test_string8));
    DoPostIncrementTest(list.begin(), test_string32);
}

TEST(UnicodeIteratorTests, Utf16PreIncrementTest)
{
    // Native encoding
    DoPreIncrementTest(empty_string16, empty_string32);
    DoPreIncrementTest(foobar_string16, foobar_string32);
    DoPreIncrementTest(test_string16, test_string32);

    //char16_t values[std::size(test_string16)] = std::move(test_string16);
    auto values = copy_array(test_string16);
    std::forward_list<char16_t> list(std::begin(values), std::end(values));
    DoPreIncrementTest(list.begin(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPreIncrementTest<dhorn::unicode::encoding::utf_16be>(empty_string16, empty_string32);
    DoPreIncrementTest<dhorn::unicode::encoding::utf_16be>(foobar_string16_be, foobar_string32);
    DoPreIncrementTest<dhorn::unicode::encoding::utf_16be>(test_string16_be, test_string32);

    for (auto& ch : values)
    {
        auto highByte = ch & 0xFF00;
        auto lowByte = ch & 0x00FF;
        ch = static_cast<char16_t>((highByte >> 8) | (lowByte << 8));
    }
    list.assign(std::begin(values), std::end(values));
    DoPreIncrementTest<dhorn::unicode::encoding::utf_16be>(list.begin(), test_string32);
}

TEST(UnicodeIteratorTests, Utf16PostIncrementTest)
{
    DoPostIncrementTest(empty_string16, empty_string32);
    DoPostIncrementTest(foobar_string16, foobar_string32);
    DoPostIncrementTest(test_string16, test_string32);

    auto values = copy_array(test_string16);
    std::forward_list<char16_t> list(std::begin(values), std::end(values));
    DoPostIncrementTest(list.begin(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPostIncrementTest<dhorn::unicode::encoding::utf_16be>(empty_string16, empty_string32);
    DoPostIncrementTest<dhorn::unicode::encoding::utf_16be>(foobar_string16_be, foobar_string32);
    DoPostIncrementTest<dhorn::unicode::encoding::utf_16be>(test_string16_be, test_string32);

    for (auto& ch : values)
    {
        auto highByte = ch & 0xFF00;
        auto lowByte = ch & 0x00FF;
        ch = static_cast<char16_t>((highByte >> 8) | (lowByte << 8));
    }
    list.assign(std::begin(values), std::end(values));
    DoPostIncrementTest<dhorn::unicode::encoding::utf_16be>(list.begin(), test_string32);
}

TEST(UnicodeIteratorTests, Utf32PreIncrementTest)
{
    // Native encoding
    DoPreIncrementTest(empty_string32, empty_string32);
    DoPreIncrementTest(foobar_string32, foobar_string32);
    DoPreIncrementTest(test_string32, test_string32);

    auto values = copy_array(test_string32);
    std::forward_list<char32_t> list(std::begin(values), std::end(values));
    DoPreIncrementTest(list.begin(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPreIncrementTest<dhorn::unicode::encoding::utf_32be>(empty_string32, empty_string32);
    DoPreIncrementTest<dhorn::unicode::encoding::utf_32be>(foobar_string32_be, foobar_string32);
    DoPreIncrementTest<dhorn::unicode::encoding::utf_32be>(test_string32_be, test_string32);

    for (auto& ch : values)
    {
        auto byte0 = ch & 0x000000FF;
        auto byte1 = ch & 0x0000FF00;
        auto byte2 = ch & 0x00FF0000;
        auto byte3 = ch & 0xFF000000;
        ch = static_cast<char32_t>((byte0 << 24) | (byte1 << 8) | (byte2 >> 8) | (byte3 >> 24));
    }
    list.assign(std::begin(values), std::end(values));
    DoPostIncrementTest<dhorn::unicode::encoding::utf_32be>(list.begin(), test_string32);
}

TEST(UnicodeIteratorTests, Utf32PostIncrementTest)
{
    // Native encoding
    DoPostIncrementTest(empty_string32, empty_string32);
    DoPostIncrementTest(foobar_string32, foobar_string32);
    DoPostIncrementTest(test_string32, test_string32);

    auto values = copy_array(test_string32);
    std::forward_list<char32_t> list(std::begin(values), std::end(values));
    DoPostIncrementTest(list.begin(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPostIncrementTest<dhorn::unicode::encoding::utf_32be>(empty_string32, empty_string32);
    DoPostIncrementTest<dhorn::unicode::encoding::utf_32be>(foobar_string32_be, foobar_string32);
    DoPostIncrementTest<dhorn::unicode::encoding::utf_32be>(test_string32_be, test_string32);

    for (auto& ch : values)
    {
        auto byte0 = ch & 0x000000FF;
        auto byte1 = ch & 0x0000FF00;
        auto byte2 = ch & 0x00FF0000;
        auto byte3 = ch & 0xFF000000;
        ch = static_cast<char32_t>((byte0 << 24) | (byte1 << 8) | (byte2 >> 8) | (byte3 >> 24));
    }
    list.assign(std::begin(values), std::end(values));
    DoPostIncrementTest<dhorn::unicode::encoding::utf_32be>(list.begin(), test_string32);
}

template <
    typename Itr,
    std::size_t Size,
    dhorn::unicode::encoding Encoding = dhorn::unicode::character_encoding_v<typename std::iterator_traits<Itr>::value_type>>
void DoPreDecrementTest(Itr end, const char32_t (&expected)[Size])
{
    dhorn::unicode::iterator<Itr, Encoding> itr(end);

    // We're at one past the end. Get to something dereferencable
    --itr;

    for (std::size_t i = Size - 1; i > 0; --i)
    {
        // Copies should be okay
        auto copy = itr;
        ASSERT_EQ(expected[i], *copy);
        ASSERT_EQ(expected[i - 1], *--copy);

        ASSERT_EQ(expected[i], *itr);
        ASSERT_EQ(expected[i - 1], *--itr);
    }

    ASSERT_EQ(expected[0], *itr);
}

template <dhorn::unicode::encoding Encoding, typename Itr, std::size_t Size>
void DoPreDecrementTest(Itr end, const char32_t (&expected)[Size])
{
    DoPreDecrementTest<Itr, Size, Encoding>(end, expected);
}

template <
    typename Itr,
    std::size_t Size,
    dhorn::unicode::encoding Encoding = dhorn::unicode::character_encoding_v<typename std::iterator_traits<Itr>::value_type>>
void DoPostDecrementTest(Itr end, const char32_t (&expected)[Size])
{
    dhorn::unicode::iterator<Itr, Encoding> itr(end);

    // We're at one past the end. Get to something dereferencable
    --itr;

    for (std::size_t i = Size - 1; i > 0; --i)
    {
        // Copies should be okay
        auto copy = itr;
        ASSERT_EQ(expected[i], *copy);
        ASSERT_EQ(expected[i], *copy--);

        ASSERT_EQ(expected[i], *itr);
        ASSERT_EQ(expected[i], *itr--);
    }

    ASSERT_EQ(expected[0], *itr);
}

template <dhorn::unicode::encoding Encoding, typename Itr, std::size_t Size>
void DoPostDecrementTest(Itr end, const char32_t (&expected)[Size])
{
    DoPostDecrementTest<Itr, Size, Encoding>(end, expected);
}

TEST(UnicodeIteratorTests, Utf8PreDecrementTest)
{
    DoPreDecrementTest(std::end(empty_string8), empty_string32);
    DoPreDecrementTest(std::end(foobar_string8), foobar_string32);
    DoPreDecrementTest(std::end(test_string8), test_string32);

    auto values = copy_array(test_string8);
    std::list<char> list(std::begin(values), std::end(values));
    DoPreDecrementTest(list.end(), test_string32);
}

TEST(UnicodeIteratorTests, Utf8PostDecrementTest)
{
    DoPostDecrementTest(std::end(empty_string8), empty_string32);
    DoPostDecrementTest(std::end(foobar_string8), foobar_string32);
    DoPostDecrementTest(std::end(test_string8), test_string32);

    auto values = copy_array(test_string8);
    std::list<char> list(std::begin(values), std::end(values));
    DoPostDecrementTest(list.end(), test_string32);
}

TEST(UnicodeIteratorTests, Utf16PreDecrementTest)
{
    // Native encoding
    DoPreDecrementTest(std::end(empty_string16), empty_string32);
    DoPreDecrementTest(std::end(foobar_string16), foobar_string32);
    DoPreDecrementTest(std::end(test_string16), test_string32);

    //char16_t values[std::size(test_string16)] = std::move(test_string16);
    auto values = copy_array(test_string16);
    std::list<char16_t> list(std::begin(values), std::end(values));
    DoPreDecrementTest(list.end(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPreDecrementTest<dhorn::unicode::encoding::utf_16be>(std::end(empty_string16), empty_string32);
    DoPreDecrementTest<dhorn::unicode::encoding::utf_16be>(std::end(foobar_string16_be), foobar_string32);
    DoPreDecrementTest<dhorn::unicode::encoding::utf_16be>(std::end(test_string16_be), test_string32);

    for (auto& ch : values)
    {
        auto highByte = ch & 0xFF00;
        auto lowByte = ch & 0x00FF;
        ch = static_cast<char16_t>((highByte >> 8) | (lowByte << 8));
    }
    list.assign(std::begin(values), std::end(values));
    DoPreDecrementTest<dhorn::unicode::encoding::utf_16be>(list.end(), test_string32);
}

TEST(UnicodeIteratorTests, Utf16PostDecrementTest)
{
    DoPostDecrementTest(std::end(empty_string16), empty_string32);
    DoPostDecrementTest(std::end(foobar_string16), foobar_string32);
    DoPostDecrementTest(std::end(test_string16), test_string32);

    auto values = copy_array(test_string16);
    std::list<char16_t> list(std::begin(values), std::end(values));
    DoPostDecrementTest(list.end(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPostDecrementTest<dhorn::unicode::encoding::utf_16be>(std::end(empty_string16), empty_string32);
    DoPostDecrementTest<dhorn::unicode::encoding::utf_16be>(std::end(foobar_string16_be), foobar_string32);
    DoPostDecrementTest<dhorn::unicode::encoding::utf_16be>(std::end(test_string16_be), test_string32);

    for (auto& ch : values)
    {
        auto highByte = ch & 0xFF00;
        auto lowByte = ch & 0x00FF;
        ch = static_cast<char16_t>((highByte >> 8) | (lowByte << 8));
    }
    list.assign(std::begin(values), std::end(values));
    DoPostDecrementTest<dhorn::unicode::encoding::utf_16be>(list.end(), test_string32);
}

TEST(UnicodeIteratorTests, Utf32PreDecrementTest)
{
    // Native encoding
    DoPreDecrementTest(std::end(empty_string32), empty_string32);
    DoPreDecrementTest(std::end(foobar_string32), foobar_string32);
    DoPreDecrementTest(std::end(test_string32), test_string32);

    auto values = copy_array(test_string32);
    std::list<char32_t> list(std::begin(values), std::end(values));
    DoPreDecrementTest(list.end(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPreDecrementTest<dhorn::unicode::encoding::utf_32be>(std::end(empty_string32), empty_string32);
    DoPreDecrementTest<dhorn::unicode::encoding::utf_32be>(std::end(foobar_string32_be), foobar_string32);
    DoPreDecrementTest<dhorn::unicode::encoding::utf_32be>(std::end(test_string32_be), test_string32);

    for (auto& ch : values)
    {
        auto byte0 = ch & 0x000000FF;
        auto byte1 = ch & 0x0000FF00;
        auto byte2 = ch & 0x00FF0000;
        auto byte3 = ch & 0xFF000000;
        ch = static_cast<char32_t>((byte0 << 24) | (byte1 << 8) | (byte2 >> 8) | (byte3 >> 24));
    }
    list.assign(std::begin(values), std::end(values));
    DoPostDecrementTest<dhorn::unicode::encoding::utf_32be>(list.end(), test_string32);
}

TEST(UnicodeIteratorTests, Utf32PostDecrementTest)
{
    // Native encoding
    DoPostDecrementTest(std::end(empty_string32), empty_string32);
    DoPostDecrementTest(std::end(foobar_string32), foobar_string32);
    DoPostDecrementTest(std::end(test_string32), test_string32);

    auto values = copy_array(test_string32);
    std::list<char32_t> list(std::begin(values), std::end(values));
    DoPostDecrementTest(list.end(), test_string32);

    // Non-native encoding. TODO: For now assume little-endian
    static_assert(dhorn::unicode::encoding::utf_16 == dhorn::unicode::encoding::utf_16le);
    DoPostDecrementTest<dhorn::unicode::encoding::utf_32be>(std::end(empty_string32), empty_string32);
    DoPostDecrementTest<dhorn::unicode::encoding::utf_32be>(std::end(foobar_string32_be), foobar_string32);
    DoPostDecrementTest<dhorn::unicode::encoding::utf_32be>(std::end(test_string32_be), test_string32);

    for (auto& ch : values)
    {
        auto byte0 = ch & 0x000000FF;
        auto byte1 = ch & 0x0000FF00;
        auto byte2 = ch & 0x00FF0000;
        auto byte3 = ch & 0xFF000000;
        ch = static_cast<char32_t>((byte0 << 24) | (byte1 << 8) | (byte2 >> 8) | (byte3 >> 24));
    }
    list.assign(std::begin(values), std::end(values));
    DoPostDecrementTest<dhorn::unicode::encoding::utf_32be>(list.end(), test_string32);
}

template <dhorn::unicode::encoding Encoding, typename CharTy>
void DoOutputIteratorTest(const char32_t* str, const CharTy* expected)
{
    std::basic_string<CharTy> result;
    auto itr = dhorn::unicode::make_output_iterator<Encoding>(std::back_inserter(result));
    for (std::size_t i = 0; str[i]; ++i)
    {
        *itr++ = str[i];
    }

    ASSERT_TRUE(result == expected);
}

TEST(UnicodeIteratorTests, Utf8OutputIteratorTest)
{
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_8>(empty_string32, empty_string8);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_8>(foobar_string32, foobar_string8);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_8>(test_string32, test_string8);
}

TEST(UnicodeIteratorTests, Utf16OutputIteratorTest)
{
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_16>(empty_string32, empty_string16);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_16>(foobar_string32, foobar_string16);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_16>(test_string32, test_string16);

    DoOutputIteratorTest<dhorn::unicode::encoding::utf_16be>(foobar_string32, foobar_string16_be);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_16be>(test_string32, test_string16_be);
}

TEST(UnicodeIteratorTests, Utf32OutputIteratorTest)
{
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_32>(empty_string32, empty_string32);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_32>(foobar_string32, foobar_string32);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_32>(test_string32, test_string32);

    DoOutputIteratorTest<dhorn::unicode::encoding::utf_32be>(foobar_string32, foobar_string32_be);
    DoOutputIteratorTest<dhorn::unicode::encoding::utf_32be>(test_string32, test_string32_be);
}
