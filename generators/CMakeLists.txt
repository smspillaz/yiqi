# /generators/CMakeLists.txt
# Build some simple code generators used to determine the
# available instrumentation tools at compile time
#
# See LICENCE.md for Copyright information

set (YIQI_GENERATORS_COMMON_LIBRARY yiqi_tools_generators_common)
set (YIQI_GENERATORS_COMMON_LIBRARY_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/generators_common.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/generators_common.h)

polysquare_add_library (${YIQI_GENERATORS_COMMON_LIBRARY} STATIC
                        SOURCES ${YIQI_GENERATORS_COMMON_LIBRARY_SRCS}
                        EXTERNAL_INCLUDE_DIRS ${YIQI_EXTERNAL_INCLUDE_DIRS}
                        LIBRARIES
                        ${YIQI_EXTERNAL_LIBRARIES})

set (YIQI_TOOLS_DECLARATIONS_GENERATOR_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_tools_declarations_generator.cpp)

polysquare_add_executable (${YIQI_TOOLS_DECLARATIONS_GENERATOR}
                           SOURCES ${YIQI_TOOLS_DECLARATIONS_GENERATOR_SRCS}
                           EXTERNAL_INCLUDE_DIRS ${YIQI_EXTERNAL_INCLUDE_DIRS}
                           LIBRARIES
                           ${YIQI_EXTERNAL_LIBRARIES}
                           ${YIQI_GENERATORS_COMMON_LIBRARY})

set (YIQI_TOOLS_FACTORY_GENERATOR_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_tools_factory_generator.cpp)

polysquare_add_executable (${YIQI_TOOLS_FACTORY_GENERATOR}
                           SOURCES ${YIQI_TOOLS_FACTORY_GENERATOR_SRCS}
                           EXTERNAL_INCLUDE_DIRS ${YIQI_EXTERNAL_INCLUDE_DIRS}
                           LIBRARIES
                           ${YIQI_EXTERNAL_LIBRARIES}
                           ${YIQI_GENERATORS_COMMON_LIBRARY})

set (YIQI_SIMPLE_STRING_REPLACEMENT_GENERATOR_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_simple_string_replacement_generator.cpp)

polysquare_add_executable (${YIQI_SIMPLE_STRING_REPLACEMENT_GENERATOR}
                           SOURCES
                           ${YIQI_SIMPLE_STRING_REPLACEMENT_GENERATOR_SRCS}
                           EXTERNAL_INCLUDE_DIRS ${YIQI_EXTERNAL_INCLUDE_DIRS}
                           LIBRARIES
                           ${YIQI_EXTERNAL_LIBRARIES}
                           ${YIQI_GENERATORS_COMMON_LIBRARY})