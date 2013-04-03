# /mocks/CMakeLists.txt
# Build mocks for some interfaces in use in Yiqi

include_directories (${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_EXTERNAL_INCLUDE_DIRS})

set (YIQI_MOCKS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_mock.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/system_api_mock.cpp)

add_library (${YIQI_MOCKS_LIBRARY}
             ${YIQI_MOCKS_SRCS})

target_link_libraries (${YIQI_MOCKS_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${GMOCK_LIBRARY}
                       ${GMOCK_MAIN_LIBRARY})
