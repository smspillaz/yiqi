# /tests/acceptance/passthrough/CMakeLists.txt
# Build a simple passthrough binary which takes its first argument, forks
# and passes it to execvpe ()
#
# See LICENCE.md for Copyright information.
include_directories (${YIQI_INTERNAL_SOURCE_DIRECTORY})

set (YIQI_PASSTHROUGH_CONSTANTS_LIBRARY_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/passthrough_constants.h
     ${CMAKE_CURRENT_SOURCE_DIR}/passthrough_constants.cpp)

add_library (${YIQI_PASSTHROUGH_CONSTANTS_LIBRARY} SHARED
             ${YIQI_PASSTHROUGH_CONSTANTS_LIBRARY_SRCS})

include_directories (${YIQI_INTERNAL_SOURCE_DIRECTORY})

set (YIQI_PASSTHROUGH_EXEC_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/passthrough.cpp)

add_executable (${YIQI_PASSTHROUGH_EXEC}
                ${YIQI_PASSTHROUGH_EXEC_SRCS})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_PASSTHROUGH_EXEC}
                                               ${YIQI_VERAPP_IMPORT_RULES}
                                               ERROR)

cppcheck_target_sources (${YIQI_PASSTHROUGH_EXEC}
                         INCLUDES
                         ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                         ${YIQI_INTERNAL_INCLUDE_DIRECTORY})

target_link_libraries (${YIQI_PASSTHROUGH_EXEC}
                       ${YIQI_PASSTHROUGH_CONSTANTS_LIBRARY}
                       ${YIQI_LIBRARY})
