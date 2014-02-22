# /include/yiqi/CMakeLists.txt
# Ensure that the yiqi headers are scanned for source code style compliance
#
# See LICENCE.md for Copyright information

set (YIQI_HEADERS
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation.h
     ${CMAKE_CURRENT_SOURCE_DIR}/environment.h)

polysquare_add_checked_sources (yiqi_headers
                                SOURCES ${YIQI_HEADERS})
