# /mocks/CMakeLists.txt
# Build mocks for some interfaces in use in Yiqi

set (YIQI_MOCKS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_mock.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_mock.h
     ${CMAKE_CURRENT_SOURCE_DIR}/system_api_mock.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/system_api_mock.h)

polysquare_add_mock (${YIQI_MOCKS_LIBRARY}
                     SOURCES ${YIQI_MOCKS_SRCS}
                     LIBRARIES ${YIQI_LIBRARY}
                     INTERNAL_INCLUDE_DIRS
                     ${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     EXTERNAL_INCLUDE_DIRS
                     ${YIQI_EXTERNAL_INCLUDE_DIRS}
                     EXPORT_HEADER_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
