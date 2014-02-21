# /samples/CMakeLists.txt
# Build Yiqi Samples and Demonstrations
#
# See LICENCE.md for Copyright information

set (YIQI_SAMPLES_BINARY yiqi_samples)

set (YIQI_SAMPLES_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_sample_basic.cpp)

polysquare_add_executable (${YIQI_SAMPLES_BINARY}
                           SOURCES ${YIQI_SAMPLES_SRCS}
                           INTERNAL_INCLUDE_DIRS
                           ${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                           ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                           EXTERNAL_INCLUDE_DIRS
                           ${YIQI_EXTERNAL_INCLUDE_DIRS}
                           LIBRARIES
                           ${GTEST_LIBRARY}
                           ${YIQI_MAIN_LIBRARY}
                           ${YIQI_LIBRARY}
                           ${CMAKE_THREAD_LIBS_INIT})