# /tests/unit/CMakeLists.txt
# Build Yiqi Integration Tests.
#
# Note that gtest_main is linked in here instead of yiqi_main, as we don't
# want to depend on a running instance of an instrumentation program

include_directories (${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_MOCKS_DIRECTORY}
                     ${YIQI_MATCHERS_DIRECTORY}
                     ${YIQI_TESTS_UTIL_DIRECTORY}
                     ${YIQI_EXTERNAL_INCLUDE_DIRS})

set (YIQI_INTEGRATION_TESTS_BINARY
     yiqi_integration_tests)

set (YIQI_INTEGRATION_TESTS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution.cpp)

add_executable (${YIQI_INTEGRATION_TESTS_BINARY}
                ${YIQI_INTEGRATION_TESTS_SRCS})

target_link_libraries (${YIQI_INTEGRATION_TESTS_BINARY}
                       ${YIQI_LIBRARY}
                       ${YIQI_MOCKS_LIBRARY}
                       ${YIQI_MATCHERS_LIBRARY}
                       ${YIQI_TESTS_UTIL_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${GTEST_MAIN_LIBRARY}
                       ${GMOCK_LIBRARY}
                       ${GMOCK_MAIN_LIBRARY})
