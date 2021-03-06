/*
 * Duncan Horn
 *
 * EventSourceTests.cpp
 *
 * Tests for the dhorn event_source class
 */
#include "stdafx.h"

#include <dhorn/experimental/event_source.h>
#include <dhorn/experimental/unique_event_cookie.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(EventSourceTests)
        {
            TEST_METHOD(SingleEventTest)
            {
                int x = 0;
                dhorn::experimental::event_source<void(void)> source;

                auto cookie = source.add([&]() { ++x; });
                ASSERT_NE(dhorn::experimental::invalid_event_cookie, cookie);
                ASSERT_EQ(static_cast<std::size_t>(1), source.size());

                source.invoke_all();
                ASSERT_EQ(1, x);
            }

            TEST_METHOD(RemoveTest)
            {
                int x = 0;
                dhorn::experimental::event_source<void(void)> source;

                auto cookie = source.add([&]() { ++x; });
                source.remove(cookie);
                ASSERT_EQ(static_cast<std::size_t>(0), source.size());

                // Should throw if we try and remove again
                try
                {
                    source.remove(cookie);
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::out_of_range &) {}
            }

            TEST_METHOD(MultipleEventInvokeOneSimpleTest)
            {
                int x = 0;
                dhorn::experimental::event_source<void(void)> source;

                source.add([&]() { ++x; });
                source.add([&]() { x += 2; });

                source.invoke_one(); // Invokes first added
                ASSERT_EQ(1, x);
            }

            TEST_METHOD(MultipleEventInvokeOneAdvancedTest)
            {
                int x = 0;
                dhorn::experimental::event_source<int(void)> source;

                source.add([&]() -> int { ++x; return x; });
                source.add([&]() -> int { x += 2; return x; });

                source.invoke_one([&](int val) { ASSERT_EQ(1, val); }); // Invokes first added
                ASSERT_EQ(1, x);
            }

            TEST_METHOD(MultipleEventInvokeAllSimpleTest)
            {
                int x = 0;
                dhorn::experimental::event_source<void(void)> source;

                source.add([&]() { ++x; });
                source.add([&]() { x += 2; });

                source.invoke_all();
                ASSERT_EQ(3, x);
            }

            TEST_METHOD(MultipleEventInvokeAllAdvancedTest)
            {
                int x = 0;
                dhorn::experimental::event_source<int(void)> source;

                auto cookie = source.add([&]() -> int { ++x; return x; });
                source.add([&]() -> int { x += 2; return x; });

                source.invoke_all([&](int val) { ASSERT_EQ(x, val); });
                ASSERT_EQ(3, x);

                source.remove(cookie);
                source.invoke_all();
                ASSERT_EQ(5, x);
            }

            TEST_METHOD(NonVoidArgsTest)
            {
                int x = 0;
                dhorn::experimental::event_source<void(int, int)> source;

                source.add([&](int a, int b)
                {
                    x += a + b;
                });

                source.invoke_all(1, 2);

                ASSERT_EQ(3, x);
            }
        };



        TEST_CLASS(UniqueEventCookieTests)
        {
            using source_type = dhorn::experimental::event_source<void(void)>;

            TEST_METHOD(DefaultConstructionTest)
            {
                // Should not throw/AV when empty
                dhorn::experimental::unique_event_cookie cookie;
            }

            TEST_METHOD(EventCookieConstuctionTest)
            {
                source_type source;
                {
                    dhorn::experimental::unique_event_cookie cookie(
                        source.add([]() {}),
                        std::bind(&source_type::remove, &source, std::placeholders::_1));

                    ASSERT_EQ(static_cast<std::size_t>(1), source.size());
                }

                // Destructor should have removed
                ASSERT_EQ(static_cast<std::size_t>(0), source.size());
            }

            TEST_METHOD(EventCookieMoveConstructionTest)
            {
                source_type source;
                {
                    dhorn::experimental::unique_event_cookie cookie(
                        source.add([]() {}),
                        std::bind(&source_type::remove, &source, std::placeholders::_1));
                    ASSERT_EQ(static_cast<std::size_t>(1), source.size());

                    dhorn::experimental::unique_event_cookie cookie2(std::move(cookie));
                    ASSERT_EQ(dhorn::experimental::invalid_event_cookie, static_cast<dhorn::experimental::event_cookie>(cookie));
                }

                // Destructor should have removed
                ASSERT_EQ(static_cast<std::size_t>(0), source.size());
            }
        };
    }
}
