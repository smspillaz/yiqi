# /testutil/CMakeLists.txt
# Build some shared functionality for the tests

include_directories (${YIQI_EXTERNAL_INCLUDE_DIRS})

set (YIQI_TESTS_UTIL_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/test_util.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/test_util.h
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution_tests_shared.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution_tests_shared.h)

add_library (${YIQI_TESTS_UTIL_LIBRARY}
             ${YIQI_TESTS_UTIL_SRCS})

target_link_libraries (${YIQI_TESTS_UTIL_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${GMOCK_LIBRARY}
                       ${GMOCK_MAIN_LIBRARY})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_TESTS_UTIL_LIBRARY}
                                               WARN_ONLY)
