# /tests/unit/CMakeLists.txt
# Build Yiqi Integration Tests.
#
# Note that yiqi_tests_main is linked in here instead of yiqi_main, as we don't
# want to depend on a running instance of an instrumentation program

set (YIQI_INTEGRATION_TESTS_BINARY
     yiqi_integration_tests)

set (YIQI_INTEGRATION_TESTS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/scopeguard.cpp)

polysquare_add_test (${YIQI_INTEGRATION_TESTS_BINARY}
                     SOURCES ${YIQI_INTEGRATION_TESTS_SRCS}
                     EXTERNAL_INCLUDE_DIRS
                     ${YIQI_EXTERNAL_INCLUDE_DIRS}
                     INTERNAL_INCLUDE_DIRS
                     ${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_TESTS_UTIL_DIRECTORY}
                     LIBRARIES
                     ${YIQI_LIBRARY}
                     ${YIQI_TESTS_UTIL_LIBRARY}
                     MATCHERS ${YIQI_MATCHERS_LIBRARY}
                     MOCKS ${YIQI_MOCKS_LIBRARY}
                     MAIN_LIBRARY ${YIQI_TESTS_MAIN_LIBRARY})
