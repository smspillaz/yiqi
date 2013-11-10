# /generators/CMakeLists.txt
# Build some simple code generators used to determine the
# available instrumentation tools at compile time
#
# See LICENCE.md for Copyright information

set (YIQI_GENERATORS_COMMON_LIBRARY yiqi_tools_generators_common)
set (YIQI_GENERATORS_COMMON_LIBRARY_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/generators_common.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/generators_common.h)

add_library (${YIQI_GENERATORS_COMMON_LIBRARY} STATIC
             ${YIQI_GENERATORS_COMMON_LIBRARY_SRCS})

target_link_libraries (${YIQI_GENERATORS_COMMON_LIBRARY}
                       ${YIQI_EXTERNAL_LIBRARIES})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_GENERATORS_COMMON_LIBRARY}
                                               ERROR)

set (YIQI_TOOLS_DECLARATIONS_GENERATOR_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_tools_declarations_generator.cpp)

add_executable (${YIQI_TOOLS_DECLARATIONS_GENERATOR}
                ${YIQI_TOOLS_DECLARATIONS_GENERATOR_SRCS})

target_link_libraries (${YIQI_TOOLS_DECLARATIONS_GENERATOR}
                       ${YIQI_GENERATORS_COMMON_LIBRARY})

set (DECLARATIONS_GENERATOR ${YIQI_TOOLS_DECLARATIONS_GENERATOR})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${DECLARATIONS_GENERATOR}
                                               ERROR)

set (YIQI_TOOLS_FACTORY_GENERATOR_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/yiqi_tools_factory_generator.cpp)

add_executable (${YIQI_TOOLS_FACTORY_GENERATOR}
                ${YIQI_TOOLS_FACTORY_GENERATOR_SRCS})

target_link_libraries (${YIQI_TOOLS_FACTORY_GENERATOR}
                       ${YIQI_GENERATORS_COMMON_LIBRARY})

set (FACTORY_GENERATOR ${YIQI_TOOLS_DECLARATIONS_GENERATOR})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${FACTORY_GENERATOR}
                                               ERROR)
