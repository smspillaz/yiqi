# /tests/unit/CMakeLists.txt
# Build Yiqi Unit Tests.
#
# Note that gtest_main is linked in here instead of yiqi_main, as we don't
# want to depend on a running instance of an instrumentation program

include_directories (${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_BINARY_DIRECTORY}
                     ${YIQI_MOCKS_DIRECTORY}
                     ${YIQI_MATCHERS_DIRECTORY}
                     ${YIQI_TESTS_UTIL_DIRECTORY}
                     ${YIQI_EXTERNAL_INCLUDE_DIRS})

set (YIQI_UNIT_TESTS_BINARY
     yiqi_unit_tests)

set (YIQI_UNIT_TESTS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/commandline.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/construction.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/constants.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/systempaths.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/value_type_test.h)

add_executable (${YIQI_UNIT_TESTS_BINARY}
                ${YIQI_UNIT_TESTS_SRCS})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_UNIT_TESTS_BINARY}
                                               ${YIQI_VERAPP_IMPORT_RULES}
                                               ERROR)

cppcheck_target_sources (${YIQI_UNIT_TESTS_BINARY}
                         INCLUDES
                         ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                         ${YIQI_INTERNAL_INCLUDE_DIRECTORY})

target_link_libraries (${YIQI_UNIT_TESTS_BINARY}
                       ${YIQI_LIBRARY}
                       ${YIQI_MOCKS_LIBRARY}
                       ${YIQI_MATCHERS_LIBRARY}
                       ${YIQI_TESTS_UTIL_LIBRARY}
                       ${GTEST_BOTH_LIBRARIES}
                       ${GMOCK_LIBRARY}
                       ${GMOCK_MAIN_LIBRARY})
