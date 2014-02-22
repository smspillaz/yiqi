# /tests/acceptance/simple_test_binary/CMakeLists.txt
# A simple test binary linked to Google Test and yiqi_main, so that we can run it
# and observe how our system behaves as a whole

set (YIQI_SIMPLE_TEST_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/simple_test.cpp)

polysquare_add_executable (${YIQI_SIMPLE_TEST_EXEC}
                           SOURCES ${YIQI_SIMPLE_TEST_SRCS}
                           EXTERNAL_INCLUDE_DIRS ${YIQI_EXTERNAL_INCLUDE_DIRS}
                           LIBRARIES
                           ${YIQI_MAIN_LIBRARY}
                           ${YIQI_LIBRARY}
                           ${GTEST_LIBRARY}
                           ${CMAKE_THREAD_LIBS_INIT})
