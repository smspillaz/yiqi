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

set (YIQI_LIBRARY_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/construction.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/constants.cpp)

add_library (${YIQI_LIBRARY} STATIC
             ${YIQI_LIBRARY_SRCS})

target_link_libraries (${YIQI_LIBRARY}
                       ${YIQI_EXTERNAL_LIBRARIES})
