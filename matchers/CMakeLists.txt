# /matchers/CMakeLists.txt
# Main CMake file for the Yiqi internal Google Test matchers
#
# See LICENCE.md for Copyright information

set (YIQI_MATCHERS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/array_fits_matchers.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/array_fits_matchers.h)

polysquare_add_matcher (${YIQI_MATCHERS_LIBRARY}
                        SOURCES ${YIQI_MATCHERS_SRCS}
                        LIBRARIES ${YIQI_LIBRARY}
                        INTERNAL_INCLUDE_DIRS
                        ${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                        ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                        EXTERNAL_INCLUDE_DIRS
                        ${YIQI_EXTERNAL_INCLUDE_DIRS}
                        EXPORT_HEADER_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
