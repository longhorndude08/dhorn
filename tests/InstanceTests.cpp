/*
 * Duncan Horn
 *
 * InstanceTests.cpp
 *
 * Tests for the dhorn instance class
 */
#include "stdafx.h"

#include <condition_variable>
#include <thread>
#include <vector>

#include <dhorn/experimental/instance.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        class test_class
        {
        public:
            test_class(void) :
                x(0)
            {
                ++instance_count;
                ++created_count;
            }

            ~test_class(void)
            {
                --instance_count;
                ++destroyed_count;
            }

            int x;

            static std::atomic_size_t instance_count;
            static std::atomic_size_t created_count;
            static std::atomic_size_t destroyed_count;
        };
        std::atomic_size_t test_class::instance_count = 0;
        std::atomic_size_t test_class::created_count = 0;
        std::atomic_size_t test_class::destroyed_count = 0;



        TEST_CLASS(InstanceTests)
        {
            TEST_METHOD_INITIALIZE(TestInit)
            {
                // Reset state
                test_class::instance_count = 0;
                test_class::created_count = 0;
                test_class::destroyed_count = 0;
            }

#pragma region GetInstance tests

            TEST_METHOD(LazyInitSharedPtrGetInstanceTest)
            {
                dhorn::experimental::lazy_init_instance_t<test_class> obj;

                // Lazy init shouldn't create until we access it
                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());

                auto ptr = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get again should give the same value
                auto other = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                ASSERT_TRUE(ptr == other);
            }

            TEST_METHOD(LazyInitRawPointerGetInstanceTest)
            {
                dhorn::experimental::lazy_init_instance_t<test_class, test_class *> obj;

                // Lazy init shouldn't create until we access it
                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());

                auto ptr = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get again should give the same value
                auto other = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                ASSERT_TRUE(ptr == other);
            }

            TEST_METHOD(EagerInitSharedPtrGetInstanceTest)
            {
                dhorn::experimental::eager_init_instance_t<test_class> obj;

                // Eager init creates the object right away
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());

                auto ptr = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get again should give the same value
                auto other = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                ASSERT_TRUE(ptr == other);
            }

            TEST_METHOD(EagerInitRawPtrGetInstanceTest)
            {
                dhorn::experimental::eager_init_instance_t<test_class, test_class *> obj;

                // Eager init creates the object right away
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());

                auto ptr = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get again should give the same value
                auto other = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                ASSERT_TRUE(ptr == other);
            }

            TEST_METHOD(AtomicExchangeSharedPtrGetInstanceTest)
            {
                dhorn::experimental::atomic_exchange_instance_t<test_class> obj;

                // Atomic exchange init shouldn't create until we access it
                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());

                auto ptr = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                Assert::IsNotNull(ptr.get());

                // Calling get again should give the same value
                auto other = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                ASSERT_TRUE(ptr == other);
            }

            TEST_METHOD(AtomicExchangeRawPtrGetInstanceTest)
            {
                dhorn::experimental::atomic_exchange_instance_t<test_class, test_class *> obj;

                // Atomic exchange init shouldn't create until we access it
                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());

                auto ptr = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                Assert::IsNotNull(ptr);

                // Calling get again should give the same value
                auto other = obj.get();
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                ASSERT_TRUE(ptr == other);
            }

#pragma endregion

#pragma region Destroy Tests

            TEST_METHOD(LazyInitSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::experimental::lazy_init_instance_t<test_class> obj;
                        ptr = obj.get();
                        ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    }

                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(0), test_class::destroyed_count.load());
                }

                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
            }

            TEST_METHOD(LazyInitRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::experimental::lazy_init_instance_t<test_class, test_class *> obj;
                        ptr = obj.get();
                        ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
                }

                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
            }

            TEST_METHOD(EagerInitSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::experimental::eager_init_instance_t<test_class> obj;
                        ptr = obj.get();
                        ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    }

                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(0), test_class::destroyed_count.load());
                }

                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
            }

            TEST_METHOD(EagerInitRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::experimental::eager_init_instance_t<test_class, test_class *> obj;
                        ptr = obj.get();
                        ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
                }

                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
            }

            TEST_METHOD(AtomicExchangeSharedPtrDestroyTest)
            {
                {
                    std::shared_ptr<test_class> ptr;
                    {
                        dhorn::experimental::atomic_exchange_instance_t<test_class> obj;
                        ptr = obj.get();
                        ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    }

                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(0), test_class::destroyed_count.load());
                }

                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
            }

            TEST_METHOD(AtomicExchangeRawPtrDestroyTest)
            {
                {
                    test_class *ptr;
                    {
                        dhorn::experimental::atomic_exchange_instance_t<test_class, test_class *> obj;
                        ptr = obj.get();
                        ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    }

                    // Even though ptr still references the object, no reference count is maintained
                    ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
                }

                ASSERT_EQ(static_cast<std::size_t>(0), test_class::instance_count.load());
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::destroyed_count.load());
            }

#pragma endregion

#pragma region Operator Overload Tests

            TEST_METHOD(LazyInitSharedPtrOperatorArrowTest)
            {
                dhorn::experimental::lazy_init_instance_t<test_class> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                ASSERT_EQ(42, obj->x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(LazyInitRawPtrOperatorArrowTest)
            {
                dhorn::experimental::lazy_init_instance_t<test_class, test_class *> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                ASSERT_EQ(42, obj->x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(LazyInitSharedPtrOperatorStarTest)
            {
                dhorn::experimental::lazy_init_instance_t<test_class> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                ASSERT_EQ(42, (*obj).x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(LazyInitRawPtrOperatorStarTest)
            {
                dhorn::experimental::lazy_init_instance_t<test_class, test_class *> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                ASSERT_EQ(42, (*obj).x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitSharedPtrOperatorArrowTest)
            {
                dhorn::experimental::eager_init_instance_t<test_class> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                ASSERT_EQ(42, obj->x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitRawPtrOperatorArrowTest)
            {
                dhorn::experimental::eager_init_instance_t<test_class, test_class *> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                ASSERT_EQ(42, obj->x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitSharedPtrOperatorStarTest)
            {
                dhorn::experimental::eager_init_instance_t<test_class> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                ASSERT_EQ(42, (*obj).x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(EagerInitRawPtrOperatorStarTest)
            {
                dhorn::experimental::eager_init_instance_t<test_class, test_class *> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                ASSERT_EQ(42, (*obj).x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeSharedPtrOperatorArrowTest)
            {
                dhorn::experimental::atomic_exchange_instance_t<test_class> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                ASSERT_EQ(42, obj->x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeRawPtrOperatorArrowTest)
            {
                dhorn::experimental::atomic_exchange_instance_t<test_class, test_class *> obj;

                // operator-> should do an implicit get/initialization
                obj->x = 42;
                ASSERT_EQ(42, obj->x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeSharedPtrOperatorStarTest)
            {
                dhorn::experimental::atomic_exchange_instance_t<test_class> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                ASSERT_EQ(42, (*obj).x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

            TEST_METHOD(AtomicExchangeRawPtrOperatorStarTest)
            {
                dhorn::experimental::atomic_exchange_instance_t<test_class, test_class *> obj;

                // operator* should do an implicit get/initialization
                (*obj).x = 42;
                ASSERT_EQ(42, (*obj).x);
                ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
            }

#pragma endregion

#pragma region Concurrent Access Tests

            static const std::size_t test_iterations = 100;
            static const std::size_t test_threads = 12;

            TEST_METHOD(LazyInitConcurrentAccessTest)
            {
                for (std::size_t i = 0; i < test_iterations; ++i)
                {
                    // Reset state
                    test_class::instance_count = 0;
                    test_class::created_count = 0;
                    test_class::destroyed_count = 0;

                    dhorn::experimental::lazy_init_instance_t<test_class> obj;
                    std::vector<std::thread> threads;
                    std::condition_variable sync;
                    std::mutex mutex;

                    std::size_t running = 0;
                    std::vector<std::shared_ptr<test_class>> pointers(test_threads);
                    for (std::size_t j = 0; j < test_threads; ++j)
                    {
                        threads.emplace_back([&, j]()
                        {
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                ++running;
                                if (running == test_threads)
                                {
                                    sync.notify_all();
                                }
                                else
                                {
                                    sync.wait(lock);
                                }
                            }

                            pointers[j] = obj.get();
                        });
                    }

                    for (std::size_t j = 0; j < test_threads; ++j)
                    {
                        threads[j].join();
                        ASSERT_TRUE(pointers[0] == pointers[j]);
                    }

                    // Only one instance should have been created
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                }
            }

            TEST_METHOD(EagerInitConcurrentAccessTest)
            {
                for (std::size_t i = 0; i < test_iterations; ++i)
                {
                    // Reset state
                    test_class::instance_count = 0;
                    test_class::created_count = 0;
                    test_class::destroyed_count = 0;

                    dhorn::experimental::eager_init_instance_t<test_class> obj;
                    std::vector<std::thread> threads;
                    std::condition_variable sync;
                    std::mutex mutex;

                    std::size_t running = 0;
                    std::vector<std::shared_ptr<test_class>> pointers(test_threads);
                    for (std::size_t j = 0; j < test_threads; ++j)
                    {
                        threads.emplace_back([&, j]()
                        {
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                ++running;
                                if (running == test_threads)
                                {
                                    sync.notify_all();
                                }
                                else
                                {
                                    sync.wait(lock);
                                }
                            }

                            pointers[j] = obj.get();
                        });
                    }

                    for (std::size_t j = 0; j < test_threads; ++j)
                    {
                        threads[j].join();
                        ASSERT_TRUE(pointers[0] == pointers[j]);
                    }

                    // Only one instance should have been created
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::created_count.load());
                }
            }

            TEST_METHOD(AtomicExchangeConcurrentAccessTest)
            {
                bool doubleAccess = false;

                for (std::size_t i = 0; i < test_iterations; ++i)
                {
                    // Reset state
                    test_class::instance_count = 0;
                    test_class::created_count = 0;
                    test_class::destroyed_count = 0;

                    dhorn::experimental::atomic_exchange_instance_t<test_class> obj;
                    std::vector<std::thread> threads;
                    std::condition_variable sync;
                    std::mutex mutex;

                    std::size_t running = 0;
                    std::vector<std::shared_ptr<test_class>> pointers(test_threads);
                    for (std::size_t j = 0; j < test_threads; ++j)
                    {
                        threads.emplace_back([&, j]()
                        {
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                ++running;
                                if (running == test_threads)
                                {
                                    sync.notify_all();
                                }
                                else
                                {
                                    sync.wait(lock);
                                }
                            }

                            pointers[j] = obj.get();
                        });
                    }

                    for (std::size_t j = 0; j < test_threads; ++j)
                    {
                        threads[j].join();
                        ASSERT_TRUE(pointers[0] == pointers[j]);
                    }

                    // May have created more than one instance, but only should still exist
                    ASSERT_EQ(static_cast<std::size_t>(1), test_class::instance_count.load());
                    if (test_class::created_count.load() > 1)
                    {
                        doubleAccess = true;
                    }
                }

                // We should have had at least one race condition
                ASSERT_TRUE(doubleAccess);
            }

#pragma endregion
        };
    }
}
