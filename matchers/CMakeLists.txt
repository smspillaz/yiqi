# /matchers/CMakeLists.txt
# Main CMake file for the Yiqi internal Google Test matchers
#
# See LICENCE.md for Copyright information

include_directories (${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_EXTERNAL_INCLUDE_DIRS})

set (YIQI_MATCHERS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/array_fits_matchers.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/array_fits_matchers.h)

add_library (${YIQI_MATCHERS_LIBRARY}
             ${YIQI_MATCHERS_SRCS})

target_link_libraries (${YIQI_MATCHERS_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${GMOCK_LIBRARY}
                       ${GMOCK_MAIN_LIBRARY})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_MATCHERS_LIBRARY}
                                               ERROR)
