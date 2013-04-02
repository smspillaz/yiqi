# /tests/unit/CMakeLists.txt
# Build Yiqi Unit Tests.
#
# Note that gtest_main is linked in here instead of yiqi_main, as we don't
# want to depend on a running instance of an instrumentation program

include_directories (${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY})

set (YIQI_UNIT_TESTS_BINARY
     yiqi_unit_tests)

set (YIQI_UNIT_TESTS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/construction.cpp)

add_executable (${YIQI_UNIT_TESTS_BINARY}
                ${YIQI_UNIT_TESTS_SRCS})

target_link_libraries (${YIQI_UNIT_TESTS_BINARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${GTEST_MAIN_LIBRARY})