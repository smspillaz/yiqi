# /CMakeLists.txt
# Main CMake file for the Yiqi project. Finds Google Test
# and determines if we can build Yiqi
#
# See LICENCE.md for Copyright information

project (YiqiInstrumentation)

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

# Search the following directories as well as
# the global cmake directories for new modules
set (YIQI_CMAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (YIQI_VERAPP_CMAKE_DIRECTORY ${YIQI_CMAKE_DIRECTORY}/Vera++)
set (YIQI_CPPCHECK_CMAKE_DIRECTORY ${YIQI_CMAKE_DIRECTORY}/CMakeCPPCheck)
set (YIQI_GOOGLE_MOCK_CMAKE_DIRECTORY
     ${YIQI_CMAKE_DIRECTORY}/CMakeGoogleMock)
set (YIQI_CPP_SUBPROCESS_CMAKE_DIRECTORY
     ${YIQI_CMAKE_DIRECTORY}/CMakeCPPSubprocess)
set (CMAKE_MODULE_PATH
     ${YIQI_VERAPP_CMAKE_DIRECTORY}
     ${YIQI_CPPCHECK_CMAKE_DIRECTORY}
     ${YIQI_GOOGLE_MOCK_CMAKE_DIRECTORY}
     ${YIQI_CPP_SUBPROCESS_CMAKE_DIRECTORY}
     ${CMAKE_MODULE_PATH})

set (YIQI_IMPORTED_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/imported)

set (Boost_USE_STATIC_LIBS ON)
set (CPP_SUBPROCESS_SOURCES_DIRECTORY ${YIQI_IMPORTED_DIRECTORY}/cpp-subprocess)

find_package (Boost 1.46 REQUIRED COMPONENTS program_options iostreams)
find_package (GoogleMock REQUIRED)
find_package (Threads REQUIRED)
find_package (VeraPP 1.2 REQUIRED)
find_package (CPPSubprocess REQUIRED)

find_program (CPPCHECK_EXECUTABLE cppcheck)

if (NOT CPPCHECK_EXECUTABLE)

    message (SEND_ERROR "cppcheck was not found")

endif (NOT CPPCHECK_EXECUTABLE)

mark_as_advanced (CPPCHECK_EXECUTABLE)

include (FindPkgConfig)
pkg_check_modules (YIQI_VALGRIND valgrind)

# Set up Vera++
set (YIQI_VERAPP_OUTPUT_DIRECTORY
     ${CMAKE_CURRENT_BINARY_DIR}/vera++)
set (YIQI_VERAPP_SCRIPTS_OUTPUT_DIRECTORY
     ${YIQI_VERAPP_OUTPUT_DIRECTORY}/scripts)
set (YIQI_VERAPP_RULES_OUTPUT_DIRECTORY
     ${YIQI_VERAPP_SCRIPTS_OUTPUT_DIRECTORY}/rules)
set (YIQI_VERAPP_PROFILES_OUTPUT_DIRECTORY
     ${YIQI_VERAPP_OUTPUT_DIRECTORY}/profiles)
set (YIQI_VERAPP_SOURCE_DIRECTORY
     ${CMAKE_CURRENT_SOURCE_DIR}/tools/vera++)
set (YIQI_VERAPP_SCRIPTS_SOURCE_DIRECTORY
    ${YIQI_VERAPP_SOURCE_DIRECTORY}/scripts)
set (YIQI_VERAPP_RULES_SOURCE_DIRECTORY
    ${YIQI_VERAPP_SCRIPTS_SOURCE_DIRECTORY}/rules/)
set (YIQI_VERAPP_PROFILES_SOURCE_DIRECTORY
    ${YIQI_VERAPP_SOURCE_DIRECTORY}/profiles/)

set (YIQI_VERAPP_PROFILE yiqi)

set (YIQI_VERAPP_IMPORT_RULES yiqi_verapp_import_rules)

include (${YIQI_CMAKE_DIRECTORY}/VeraPPTargets.cmake)

# Set up cppcheck
include (CPPCheck)

# -fPIC, -Wall and -Werror are mandatory
set (COMPILER_FLAGS "-fPIC -Wall -Werror")
set (CXX_CXX11_FLAGS "-std=c++0x")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} ${CXX_CXX11_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}")

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
     ${Boost_LIBRARIES}
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

set (YIQI_MAIN_LIBRARY yiqi_main)
set (YIQI_LIBRARY yiqi)

# Set up Generator Names and Paths
set (YIQI_TOOLS_DECLARATIONS_GENERATOR yiqi_tools_declarations_generator)
set (YIQI_TOOLS_DECLARATIONS_GENERATOR_PATH
     ${YIQI_GENERATORS_BINARY_DIRECTORY}/${YIQI_TOOLS_DECLARATIONS_GENERATOR})

set (YIQI_TOOLS_FACTORY_GENERATOR yiqi_tools_factory_generator)
set (YIQI_TOOLS_FACTORY_GENERATOR_PATH
     ${YIQI_GENERATORS_BINARY_DIRECTORY}/${YIQI_TOOLS_FACTORY_GENERATOR})

add_subdirectory (${YIQI_INTERNAL_INCLUDE_DIRECTORY})
add_subdirectory (${YIQI_INTERNAL_SOURCE_DIRECTORY})
add_subdirectory (${YIQI_GENERATORS_DIRECTORY})
add_subdirectory (${YIQI_SAMPLES_DIRECTORY})
add_subdirectory (${YIQI_TESTS_DIRECTORY})
add_subdirectory (${YIQI_MOCKS_DIRECTORY})
add_subdirectory (${YIQI_MATCHERS_DIRECTORY})
add_subdirectory (${YIQI_TESTS_UTIL_DIRECTORY})

# Append all sources to unused function check
add_custom_target (yiqi_check_unused ALL
                   COMMENT "Checking for unused functions")

# The boost and gtest libraries are quite complicated 
set (YIQI_UNUSED_INCLUDES
     ${YIQI_INTERNAL_INCLUDE_DIRECTORY}
     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
     ${YIQI_INTERNAL_SOURCE_BINARY_DIRECTORY})
cppcheck_add_global_unused_function_check_to_target (yiqi_check_unused
                                                     INCLUDES
                                                     ${YIQI_UNUSED_INCLUDES})

# Get available tools and print report
set (AVAILABLE_TOOLS)
yiqi_tools_available (AVAILABLE_TOOLS)

message (STATUS "Yiqi will be compiled with support for the following tools:")

foreach (TOOL ${AVAILABLE_TOOLS})
    message (STATUS "* " ${TOOL})
endforeach ()
