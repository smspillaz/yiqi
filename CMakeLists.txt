# /CMakeLists.txt
# Main CMake file for the Yiqi project. Finds Google Test
# and determines if we can build Yiqi
#
# See LICENCE.md for Copyright information

if (POLICY CMP0025)

    cmake_policy (SET CMP0025 NEW)

endif (POLICY CMP0025)

project (YiqiInstrumentation)
cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

option (YIQI_BOOST_STATIC_LIBS ON
        "Link static libraries for boost. OFF if not using system compiler")

# Search the following directories as well as
# the global cmake directories for new modules
set (YIQI_CMAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (YIQI_CPPSUBPROCESS_CMAKE_DIRECTORY
     ${YIQI_CMAKE_DIRECTORY}/CMakeCPPSubprocess)
set (YIQI_POLYSQUARE_CMAKE_DIRECTORY
     ${YIQI_CMAKE_DIRECTORY}/common)

set (CMAKE_MODULE_PATH
     ${YIQI_CMAKE_DIRECTORY}
     ${YIQI_CPPSUBPROCESS_CMAKE_DIRECTORY}
     ${YIQI_POLYSQUARE_CMAKE_DIRECTORY}
     ${CMAKE_MODULE_PATH})

# Set the directory of imported modules
set (YIQI_IMPORTED_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/imported)

include (PolysquareCommon)

polysquare_compiler_bootstrap ()
polysquare_rules_bootstrap (${YIQI_POLYSQUARE_CMAKE_DIRECTORY}
                            ${CMAKE_BINARY_DIR})
polysquare_gmock_bootstrap (${YIQI_POLYSQUARE_CMAKE_DIRECTORY})
polysquare_coverage_bootstrap (${YIQI_POLYSQUARE_CMAKE_DIRECTORY})
polysquare_cotire_bootstrap (${YIQI_POLYSQUARE_CMAKE_DIRECTORY})

include (FindPkgConfig)
pkg_check_modules (YIQI_VALGRIND valgrind)

set (Boost_USE_STATIC_LIBS ${YIQI_BOOST_USE_STATIC_LIBS})
set (CPP_SUBPROCESS_SOURCES_DIRECTORY
     ${YIQI_IMPORTED_DIRECTORY}/cpp-subprocess)

function (yiqi_import_external_libraries_from_boost RETURN_VALUE
                                                    VERSION)

    cmake_parse_arguments (EXTERNAL
                           ""
                           ""
                           "COMPONENTS"
                           ${ARGN})

    find_package (Boost ${VERSION} COMPONENTS ${EXTERNAL_COMPONENTS})

    set (RETURN_LIST)

    foreach (LIBRARY ${Boost_LIBRARIES})

        list (FIND Boost_LIBRARIES ${LIBRARY} INDEX)
        list (GET EXTERNAL_COMPONENTS ${INDEX} EXT_COMPONENT_NAME)
        set (IMPORTED_LIBRARY_NAME imported_boost_${EXT_COMPONENT_NAME})

        add_library (${IMPORTED_LIBRARY_NAME} STATIC IMPORTED GLOBAL)
        set_target_properties (${IMPORTED_LIBRARY_NAME}
                               PROPERTIES
                               IMPORTED_LOCATION
                               ${LIBRARY})
        list (APPEND RETURN_LIST ${IMPORTED_LIBRARY_NAME})

    endforeach ()

    set (${RETURN_VALUE} ${RETURN_LIST} PARENT_SCOPE)

endfunction ()

find_package (Threads REQUIRED)
find_package (CPPSubprocess REQUIRED)
yiqi_import_external_libraries_from_boost (BOOST_LIBRARIES
                                           1.46
                                           COMPONENTS
                                           program_options
                                           iostreams)

# Set up the tool tracker
include (${YIQI_CMAKE_DIRECTORY}/ToolTracker.cmake)

yiqi_add_available_tool ("passthrough")
yiqi_add_available_tool ("none")
yiqi_add_available_tool ("timer")

if (YIQI_VALGRIND_FOUND)

    yiqi_add_available_tool ("memcheck")
    yiqi_add_available_tool ("callgrind")
    yiqi_add_available_tool ("cachegrind")

endif (YIQI_VALGRIND_FOUND)

set (YIQI_EXTERNAL_INCLUDE_DIRS
     ${YIQI_VALGRIND_INCLUDE_DIRS}
     ${Boost_INCLUDE_DIRS}
     ${GTEST_INCLUDE_DIR}
     ${GMOCK_INCLUDE_DIR}
     ${CPP_SUBPROCESS_INCLUDE_DIR})
set (YIQI_EXTERNAL_LIBRARY_DIRS
     ${YIQI_VALGRIND_LIBRARY_DIRS}
     ${Boost_LIBRARY_DIRS})
set (YIQI_EXTERNAL_LIBRARIES
     ${BOOST_LIBRARIES}
     ${CPP_SUBPROCESS_LIBRARY})
set (YIQI_INTERNAL_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set (YIQI_INTERNAL_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (YIQI_INTERNAL_SOURCE_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src)
set (YIQI_SAMPLES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sample)
set (YIQI_TESTS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set (YIQI_MOCKS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
set (YIQI_MATCHERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/matchers)
set (YIQI_TESTS_UTIL_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testsutil)
set (YIQI_GENERATORS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/generators)
set (YIQI_GENERATORS_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generators)

# Set up Library Names
set (YIQI_MOCKS_LIBRARY yiqi_mocks)
set (YIQI_MATCHERS_LIBRARY yiqi_matchers)
set (YIQI_TESTS_UTIL_LIBRARY yiqi_tests_util)
set (YIQI_TESTS_MAIN_LIBRARY yiqi_tests_main)

set (YIQI_MAIN_LIBRARY yiqi_main)
set (YIQI_LIBRARY yiqi)

# Set up Generator Names and Paths
set (YIQI_TOOLS_DECLARATIONS_GENERATOR yiqi_tools_declarations_generator)
set (YIQI_TOOLS_FACTORY_GENERATOR yiqi_tools_factory_generator)
set (YIQI_SIMPLE_STRING_REPLACEMENT_GENERATOR yiqi_simple_string_replacement_generator)

# Set up TAP reporter
set (YIQI_IMPORTED_GTEST_TAP_LISTENER_DIRECTORY
     ${YIQI_IMPORTED_DIRECTORY}/gtest-tap-listener)
set (YIQI_IMPORTED_GTEST_TAP_LISTENER_INCLUDE_DIRECTORY
     ${YIQI_IMPORTED_GTEST_TAP_LISTENER_DIRECTORY}/src)

add_subdirectory (${YIQI_INTERNAL_SOURCE_DIRECTORY})
add_subdirectory (${YIQI_INTERNAL_INCLUDE_DIRECTORY})
add_subdirectory (${YIQI_GENERATORS_DIRECTORY})
add_subdirectory (${YIQI_SAMPLES_DIRECTORY})
add_subdirectory (${YIQI_MOCKS_DIRECTORY})
add_subdirectory (${YIQI_MATCHERS_DIRECTORY})
add_subdirectory (${YIQI_TESTS_UTIL_DIRECTORY})
add_subdirectory (${YIQI_TESTS_DIRECTORY})

polysquare_cppcheck_complete_scanning ()

# Get available tools and print report
set (AVAILABLE_TOOLS)
yiqi_tools_available (AVAILABLE_TOOLS)

message (STATUS "Yiqi will be compiled with support for the following tools:")

foreach (TOOL ${AVAILABLE_TOOLS})
    message (STATUS "* " ${TOOL})
endforeach ()
