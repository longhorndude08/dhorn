cmake_minimum_required(VERSION 3.12)

include(../cmake/build_flags.cmake)

project(efficiency_tests)

set(SOURCES
    main.cpp
    inplace_function_tests.cpp
    unicode_tests.cpp
    vector_baseline_tests.cpp
    vector_tests.cpp)

set(INCLUDES
    ${DHORN_INCLUDE_PATH}
    ../benchmark/include)

add_executable(efficiency_tests)
target_compile_options(efficiency_tests PUBLIC -Wno-unused-variable)
target_sources(efficiency_tests PUBLIC ${SOURCES})
target_include_directories(efficiency_tests PUBLIC ${INCLUDES})
target_link_libraries(efficiency_tests benchmark)
