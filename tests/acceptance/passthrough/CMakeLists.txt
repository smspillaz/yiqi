# /tests/acceptance/passthrough/CMakeLists.txt
# Build a simple passthrough binary which takes its first argument, forks
# and passes it to execvpe ()
#
# See LICENCE.md for Copyright information.
set (YIQI_PASSTHROUGH_CONSTANTS_LIBRARY_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/passthrough_constants.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/passthrough_constants.h)

polysquare_add_library (${YIQI_PASSTHROUGH_CONSTANTS_LIBRARY} SHARED
                        SOURCES ${YIQI_PASSTHROUGH_CONSTANTS_LIBRARY_SRCS}
                        INTERNAL_INCLUDE_DIRS ${YIQI_INTERNAL_SOURCE_DIRECTORY})

set (YIQI_PASSTHROUGH_EXEC_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/passthrough.cpp)

polysquare_add_executable (${YIQI_PASSTHROUGH_EXEC}
                           SOURCES ${YIQI_PASSTHROUGH_EXEC_SRCS}
                           INTERNAL_INCLUDE_DIRS ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                           LIBRARIES
                           ${YIQI_PASSTHROUGH_CONSTANTS_LIBRARY}
                           ${YIQI_LIBRARY})
