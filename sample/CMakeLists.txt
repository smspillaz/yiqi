# /samples/CMakeLists.txt
# Build Yiqi Samples and Demonstrations
#
# See LICENCE.md for Copyright information

set (YIQI_SAMPLES_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_sample_basic.cpp)

set (YIQI_SAMPLES_BINARY
     yiqi_samples)

add_executable (${YIQI_SAMPLES_BINARY}
                ${YIQI_SAMPLES_SRCS})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_SAMPLES_BINARY}
                                               ERROR)

target_link_libraries (${YIQI_SAMPLES_BINARY}
                       ${YIQI_MAIN_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY})
