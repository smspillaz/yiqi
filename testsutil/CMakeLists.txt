# /testutil/CMakeLists.txt
# Build some shared functionality for the tests

set (YIQI_TESTS_UTIL_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/test_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/test_util.h
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution_tests_shared.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution_tests_shared.h)

polysquare_add_library (${YIQI_TESTS_UTIL_LIBRARY} STATIC
                        SOURCES ${YIQI_TESTS_UTIL_SRCS}
                        EXTERNAL_INCLUDE_DIRS
                        ${YIQI_IMPORTED_GTEST_TAP_LISTENER_INCLUDE_DIRECTORY}
                        ${YIQI_EXTERNAL_INCLUDE_DIRS}
                        LIBRARIES
                        ${GTEST_LIBRARY}
                        ${GMOCK_LIBRARY})

# Set up TAP listener
set (YIQI_TESTS_MAIN_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_tests_main.cpp)

add_definitions (-DGTEST_TAP_PRINT_TO_STDOUT)
polysquare_add_test_main (${YIQI_TESTS_MAIN_LIBRARY}
                          SOURCES ${YIQI_TESTS_MAIN_SRCS}
                          EXTERNAL_INCLUDE_DIRS ${YIQI_EXTERNAL_INCLUDE_DIRS})