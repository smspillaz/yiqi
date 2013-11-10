# /tests/acceptance/simple_test_binary/CMakeLists.txt
# A simple test binary linked to Google Test and yiqi_main, so that we can run it
# and observe how our system behaves as a whole

set (YIQI_SIMPLE_TEST_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/simple_test.cpp)

include_directories (${YIQI_EXTERNAL_INCLUDE_DIRS})

add_executable (${YIQI_SIMPLE_TEST_EXEC}
                ${YIQI_SIMPLE_TEST_SRCS})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_SIMPLE_TEST_EXEC}
                                               ERROR)

target_link_libraries (${YIQI_SIMPLE_TEST_EXEC}
                       ${YIQI_MAIN_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${CMAKE_THREAD_LIBS_INIT})
