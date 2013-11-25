# /mocks/CMakeLists.txt
# Build mocks for some interfaces in use in Yiqi

include_directories (${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_EXTERNAL_INCLUDE_DIRS})

set (YIQI_MOCKS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_mock.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_mock.h
     ${CMAKE_CURRENT_SOURCE_DIR}/system_api_mock.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/system_api_mock.h)

add_library (${YIQI_MOCKS_LIBRARY}
             ${YIQI_MOCKS_SRCS})

target_link_libraries (${YIQI_MOCKS_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${GMOCK_LIBRARY}
                       ${GMOCK_MAIN_LIBRARY})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_MOCKS_LIBRARY}
                                               ERROR)

cppcheck_target_sources (${YIQI_MOCKS_LIBRARY}
                        INCLUDES
                        ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                        ${YIQI_INTERNAL_INCLUDE_DIRECTORY})