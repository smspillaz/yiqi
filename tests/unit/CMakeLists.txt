# /tests/unit/CMakeLists.txt
# Build Yiqi Unit Tests.
#
# Note that gtest_main is linked in here instead of yiqi_main, as we don't
# want to depend on a running instance of an instrumentation program

set (YIQI_UNIT_TESTS_BINARY
     yiqi_unit_tests)

set (YIQI_UNIT_TESTS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/commandline.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/construction.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/constants.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/systempaths.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/value_type_test.h)

polysquare_add_test (${YIQI_UNIT_TESTS_BINARY}
                     SOURCES ${YIQI_UNIT_TESTS_SRCS}
                     EXTERNAL_INCLUDE_DIRS
                     ${YIQI_EXTERNAL_INCLUDE_DIRS}
                     INTERNAL_INCLUDE_DIRS
                     ${YIQI_INTERNAL_SOURCE_BINARY_DIRECTORY}
                     ${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_TESTS_UTIL_DIRECTORY}
                     LIBRARIES
                     ${YIQI_LIBRARY}
                     ${YIQI_TESTS_UTIL_LIBRARY}
                     MATCHERS ${YIQI_MATCHERS_LIBRARY}
                     MOCKS ${YIQI_MOCKS_LIBRARY}
                     MAIN_LIBRARY ${YIQI_TESTS_MAIN_LIBRARY})
