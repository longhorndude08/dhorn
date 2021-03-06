/*
 * Duncan Horn
 *
 * JSONTests.cpp
 *
 * Tests for json.h
 */
#include "stdafx.h"

#include <dhorn/experimental/json.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(JsonTests)
        {
#pragma region dhorn::experimental::json_cast Tests

            TEST_METHOD(Int32JsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"-42";

                auto value = std::make_shared<dhorn::experimental::json_number>(str);
                auto num = dhorn::experimental::json_cast<std::int32_t>(value.get());
                ASSERT_EQ(-42, num);
            }

            TEST_METHOD(UInt32JsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"42";

                auto value = std::make_shared<dhorn::experimental::json_number>(str);
                auto num = dhorn::experimental::json_cast<std::uint32_t>(value.get());
                ASSERT_EQ(42u, num);
            }

            TEST_METHOD(FloatJsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"42.123";

                auto value = std::make_shared<dhorn::experimental::json_number>(str);
                auto num = dhorn::experimental::json_cast<float>(value.get());
                ASSERT_EQ(42.123f, num);
            }

            TEST_METHOD(DoubleJsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"42.123";

                auto value = std::make_shared<dhorn::experimental::json_number>(str);
                auto num = dhorn::experimental::json_cast<double>(value.get());
                ASSERT_EQ(42.123, num);
            }

            TEST_METHOD(Utf8StdStringJsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"Test String";

                auto value = std::make_shared<dhorn::experimental::json_string>(str);
                auto string = dhorn::experimental::json_cast<std::string>(value.get());
                ASSERT_TRUE(str == string.c_str());
            }

            TEST_METHOD(Utf16StdStringJsonCastTest)
            {
                 dhorn::experimental::utf16_string str = u"Test String";

                auto value = std::make_shared<dhorn::experimental::json_string>(str);
                auto string = dhorn::experimental::json_cast<std::basic_string<char16_t>>(value.get());
                ASSERT_TRUE(str == string.c_str());
            }

            TEST_METHOD(Utf32StdStringJsonCastTest)
            {
                 dhorn::experimental::utf32_string str = U"Test String";

                auto value = std::make_shared<dhorn::experimental::json_string>(str);
                auto string = dhorn::experimental::json_cast<std::basic_string<char32_t>>(value.get());
                ASSERT_TRUE(str == string.c_str());
            }

            TEST_METHOD(Utf8StringJsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"Test String";

                auto value = std::make_shared<dhorn::experimental::json_string>(str);
                auto string = dhorn::experimental::json_cast< dhorn::experimental::utf8_string>(value.get());
                ASSERT_TRUE(str == string);
            }

            TEST_METHOD(Utf16StringJsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"Test String";

                auto value = std::make_shared<dhorn::experimental::json_string>(str);
                auto string = dhorn::experimental::json_cast< dhorn::experimental::utf16_string>(value.get());
                ASSERT_TRUE(str == string);
            }

            TEST_METHOD(Utf32StringJsonCastTest)
            {
                 dhorn::experimental::utf8_string str = u8"Test String";

                auto value = std::make_shared<dhorn::experimental::json_string>(str);
                auto string = dhorn::experimental::json_cast< dhorn::experimental::utf32_string>(value.get());
                ASSERT_TRUE(str == string);
            }

            TEST_METHOD(StdVectorJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::vector< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(array.size(), arr.size());
                ASSERT_TRUE(arr[0] == str1);
                ASSERT_TRUE(arr[1] == str2);
                ASSERT_TRUE(arr[2] == str3);
            }

            TEST_METHOD(StdListJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::list< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(array.size(), arr.size());

                auto itr = arr.begin();
                ASSERT_TRUE(*itr == str1);
                ++itr;
                ASSERT_TRUE(*itr == str2);
                ++itr;
                ASSERT_TRUE(*itr == str3);
            }

            TEST_METHOD(StdForwardListJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::forward_list< dhorn::experimental::utf8_string>>(value.get());

                auto itr = arr.begin();
                ASSERT_TRUE(*itr == str1);
                ++itr;
                ASSERT_TRUE(*itr == str2);
                ++itr;
                ASSERT_TRUE(*itr == str3);

                ++itr;
                ASSERT_TRUE(itr == arr.end());
            }

            TEST_METHOD(StdSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::set< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), arr.size());

                ASSERT_TRUE(arr.find(str1) != arr.end());
                ASSERT_TRUE(arr.find(str2) != arr.end());
                ASSERT_TRUE(arr.find(str3) != arr.end());

                // set does not allow duplicates
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                value = std::make_shared<dhorn::experimental::json_array>(array);
                arr = dhorn::experimental::json_cast<std::set< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), arr.size());
            }

            TEST_METHOD(StdMultiSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::multiset< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), arr.size());

                ASSERT_TRUE(arr.find(str1) != arr.end());
                ASSERT_TRUE(arr.find(str2) != arr.end());
                ASSERT_TRUE(arr.find(str3) != arr.end());

                // multiset allows duplicates
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                value = std::make_shared<dhorn::experimental::json_array>(array);
                arr = dhorn::experimental::json_cast<std::multiset< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(4), arr.size());
            }

            TEST_METHOD(StdUnorderedSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::unordered_set< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), arr.size());

                ASSERT_TRUE(arr.find(str1) != arr.end());
                ASSERT_TRUE(arr.find(str2) != arr.end());
                ASSERT_TRUE(arr.find(str3) != arr.end());

                // unordered_set does not allow duplicates
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                value = std::make_shared<dhorn::experimental::json_array>(array);
                arr = dhorn::experimental::json_cast<std::unordered_set< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), arr.size());
            }

            TEST_METHOD(StdUnorderedMultiSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::unordered_multiset< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), arr.size());

                ASSERT_TRUE(arr.find(str1) != arr.end());
                ASSERT_TRUE(arr.find(str2) != arr.end());
                ASSERT_TRUE(arr.find(str3) != arr.end());

                // unordered_multiset allows duplicates
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                value = std::make_shared<dhorn::experimental::json_array>(array);
                arr = dhorn::experimental::json_cast<std::unordered_multiset< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(4), arr.size());
            }

            TEST_METHOD(StdStackJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto stack = dhorn::experimental::json_cast<std::stack< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), stack.size());

                ASSERT_TRUE(stack.top() == str3);
                stack.pop();
                ASSERT_TRUE(stack.top() == str2);
                stack.pop();
                ASSERT_TRUE(stack.top() == str1);
                stack.pop();
            }

            TEST_METHOD(StdQueueJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto queue = dhorn::experimental::json_cast<std::queue< dhorn::experimental::utf8_string>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), queue.size());

                ASSERT_TRUE(queue.front() == str1);
                queue.pop();
                ASSERT_TRUE(queue.front() == str2);
                queue.pop();
                ASSERT_TRUE(queue.front() == str3);
                queue.pop();
            }

            TEST_METHOD(StdPriorityQueueJsonCastTest)
            {
                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_number>(u8"2"));
                array.push_back(std::make_shared<dhorn::experimental::json_number>(u8"1"));
                array.push_back(std::make_shared<dhorn::experimental::json_number>(u8"3"));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto queue = dhorn::experimental::json_cast<std::priority_queue<int>>(value.get());
                ASSERT_EQ(static_cast<std::size_t>(3), queue.size());

                // By default, priority_queue gives highest-value-first
                ASSERT_EQ(3, queue.top());
                queue.pop();
                ASSERT_EQ(2, queue.top());
                queue.pop();
                ASSERT_EQ(1, queue.top());
                queue.pop();
            }

            TEST_METHOD(StdArrayJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str1));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str2));
                array.push_back(std::make_shared<dhorn::experimental::json_string>(str3));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto arr = dhorn::experimental::json_cast<std::array< dhorn::experimental::utf8_string, 3u>>(value.get());
                ASSERT_EQ(array.size(), arr.size());
                ASSERT_TRUE(arr[0] == str1);
                ASSERT_TRUE(arr[1] == str2);
                ASSERT_TRUE(arr[2] == str3);

                try
                {
                    auto arr2 = dhorn::experimental::json_cast<std::array< dhorn::experimental::utf8_string, 4u>>(value.get());
                    Assert::Fail(L"Expected an exception");
                }
                catch (const dhorn::experimental::json_exception &)
                {
                }
            }

            TEST_METHOD(StdMapFromArraysJsonCastTest)
            {
                using pair_array = std::vector<std::shared_ptr<dhorn::experimental::json_value>>;
                std::vector<pair_array> values =
                {
                    {
                        std::make_shared<dhorn::experimental::json_number>(u8"0"),
                        std::make_shared<dhorn::experimental::json_string>(u8"String 1")
                    },
                    {
                        std::make_shared<dhorn::experimental::json_number>(u8"1"),
                        std::make_shared<dhorn::experimental::json_string>(u8"String 2")
                    },
                    {
                        std::make_shared<dhorn::experimental::json_number>(u8"2"),
                        std::make_shared<dhorn::experimental::json_string>(u8"String 3")
                    }
                };

                std::vector<std::shared_ptr<dhorn::experimental::json_value>> array;
                array.emplace_back(std::make_shared<dhorn::experimental::json_array>(values[0]));
                array.emplace_back(std::make_shared<dhorn::experimental::json_array>(values[1]));
                array.emplace_back(std::make_shared<dhorn::experimental::json_array>(values[2]));

                auto value = std::make_shared<dhorn::experimental::json_array>(array);
                auto map = dhorn::experimental::json_cast<std::map<int,  dhorn::experimental::utf8_string>>(value.get());

                ASSERT_EQ(array.size(), map.size());

                for (auto &pair : map)
                {
                    auto &jsonValue = values[pair.first][1];
                    auto jsonString = jsonValue->as<dhorn::experimental::json_string>();
                    ASSERT_TRUE(jsonString->str() == pair.second);
                }
            }

#pragma endregion
        };
    }
}
