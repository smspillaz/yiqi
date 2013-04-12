# /src/CMakeLists.txt
# Build the Yiqi main() library and the Yiqi Library
#
# See LICENCE.md for Copyright information

include_directories (${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_EXTERNAL_INCLUDE_DIRS})

set (YIQI_MAIN_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_main.cpp)

add_library (${YIQI_MAIN_LIBRARY} STATIC
             ${YIQI_MAIN_SRCS})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_MAIN_LIBRARY}
                                               WARN_ONLY)

set (YIQI_LIBRARY_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/commandline.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/construction.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/constants.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/exceptioncleanup.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_tool_valgrind_base.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_tools_available.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_none.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_timer.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_memcheck.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_callgrind.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_cachegrind.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation_passthrough.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecution.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/systempaths.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/system_unix.cpp)

add_library (${YIQI_LIBRARY} STATIC
             ${YIQI_LIBRARY_SRCS})

target_link_libraries (${YIQI_LIBRARY}
                       ${YIQI_EXTERNAL_LIBRARIES})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_LIBRARY}
                                               WARN_ONLY)
