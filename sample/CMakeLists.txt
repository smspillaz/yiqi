# /samples/CMakeLists.txt
# Build Yiqi Samples and Demonstrations
#
# See LICENCE.md for Copyright information

set (YIQI_SAMPLES_SRCS
     yiqi_sample_basic.cpp)

set (YIQI_SAMPLES_BINARY
     yiqi_samples)

add_executable (${YIQI_SAMPLES_BINARY}
                ${YIQI_SAMPLES_SRCS})

target_link_libraries (${YIQI_SAMPLES_BINARY}
                       ${YIQI_MAIN_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY})
