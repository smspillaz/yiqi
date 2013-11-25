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
set (CMAKE_MODULE_PATH
     ${YIQI_VERAPP_CMAKE_DIRECTORY}
     ${YIQI_CPPCHECK_CMAKE_DIRECTORY}
     ${CMAKE_MODULE_PATH})

set (Boost_USE_STATIC_LIBS ON)

find_package (Boost 1.46 REQUIRED COMPONENTS program_options iostreams)
find_package (GTest QUIET)
find_package (Threads REQUIRED)
find_package (VeraPP 1.2 REQUIRED)

find_program (CPPCHECK_EXECUTABLE cppcheck)

if (NOT CPPCHECK_EXECUTABLE)

    message (SEND_ERROR "cppcheck was not found")

endif (NOT CPPCHECK_EXECUTABLE)

mark_as_advanced (CPPCHECK_EXECUTABLE)

include (FindPkgConfig)
pkg_check_modules (YIQI_VALGRIND valgrind)

# We must try to find google-mock first, if that is unavailable then
# building gmock will build both it and google-test
if (GTEST_FOUND)
    find_library (GMOCK_LIBRARY gmock)
    find_library (GMOCK_MAIN_LIBRARY gmock_main)
endif (GTEST_FOUND)

if (NOT GMOCK_LIBRARY OR NOT GMOCK_MAIN_LIBRARY)

    set (GMOCK_INCLUDE_BASE "include/")

    # Find the Google Mock include directory by
    # searching the system paths
    find_path (GMOCK_INCLUDE_DIR
               gmock/gmock.h)

    if (GMOCK_INCLUDE_DIR)

        string (LENGTH ${GMOCK_INCLUDE_BASE} GMOCK_INCLUDE_BASE_LENGTH)
        string (LENGTH ${GMOCK_INCLUDE_DIR} GMOCK_INCLUDE_DIR_LENGTH)

        math (EXPR
              GMOCK_INCLUDE_PREFIX_LENGTH
              "${GMOCK_INCLUDE_DIR_LENGTH} - ${GMOCK_INCLUDE_BASE_LENGTH}")
        string (SUBSTRING
                ${GMOCK_INCLUDE_DIR}
                0
                ${GMOCK_INCLUDE_PREFIX_LENGTH}
                GMOCK_INCLUDE_PREFIX)

        find_path (GMOCK_SRC_DIR
                   CMakeLists.txt
                   PATHS ${GMOCK_INCLUDE_PREFIX}/src/gmock
                   NO_DEFAULT_PATH)

        if (GMOCK_SRC_DIR)

            set (GMOCK_INCLUDE_DIR ${GMOCK_INCLUDE_DIR})
            set (GTEST_INCLUDE_DIR ${GMOCK_SRC_DIR}/gtest/include)

            add_subdirectory (${GMOCK_SRC_DIR}
                              ${CMAKE_CURRENT_BINARY_DIR}/src/gmock)

            set (GMOCK_LIBRARY gmock)
            set (GMOCK_MAIN_LIBRARY gmock_main)
            set (GTEST_LIBRARY gtest)
            set (GTEST_MAIN_LIBRARY gmock_main)
            set (GTEST_BOTH_LIBRARIES
                 ${CMAKE_THREAD_LIBS_INIT}
                 ${GTEST_LIBRARY}
                 ${GTEST_MAIN_LIBRARY})
            set (GTEST_FOUND TRUE)

        endif (GMOCK_SRC_DIR)

    endif (GMOCK_INCLUDE_DIR)

endif (NOT GMOCK_LIBRARY OR NOT GMOCK_MAIN_LIBRARY)

if (NOT GTEST_FOUND)

    # Find the the Google Test include directory
    # by searching the system-wide include directory
    # paths
    find_path (GTEST_INCLUDE_DIR
               gtest/gtest.h)

    if (GTEST_INCLUDE_DIR)
        set (GTEST_INCLUDE_BASE "include/")
        string (LENGTH ${GTEST_INCLUDE_BASE} GTEST_INCLUDE_BASE_LENGTH)
        string (LENGTH ${GTEST_INCLUDE_DIR} GTEST_INCLUDE_DIR_LENGTH)

        math (EXPR
              GTEST_INCLUDE_PREFIX_LENGTH
              "${GTEST_INCLUDE_DIR_LENGTH} - ${GTEST_INCLUDE_BASE_LENGTH}")
        string (SUBSTRING
                ${GTEST_INCLUDE_DIR}
                0
                ${GTEST_INCLUDE_PREFIX_LENGTH}
                GTEST_INCLUDE_PREFIX)

        find_path (GTEST_SRC_DIR
                   CMakeLists.txt
                   PATHS ${GTEST_INCLUDE_PREFIX}/src/gtest
                   NO_DEFAULT_PATH)

        if (GTEST_SRC_DIR)

            add_subdirectory (${GTEST_SRC_DIR}
                              ${CMAKE_CURRENT_BINARY_DIR}/src/gtest)

            set (GTEST_LIBRARY gtest)
            set (GTEST_MAIN_LIBRARY gtest_main)
            set (GTEST_BOTH_LIBRARIES
                 ${CMAKE_THREAD_LIBS_INIT}
                 ${GTEST_LIBRARY}
                 ${GTEST_MAIN_LIBRARY})

            find_library (GMOCK_LIBRARY gmock)
            find_library (GMOCK_MAIN_LIBRARY gmock_main)

            if (GMOCK_LIBRARY AND GMOCK_MAIN_LIBRARY)

                set (GTEST_FOUND TRUE)

            endif (GMOCK_LIBRARY AND GMOCK_MAIN_LIBRARY)

        endif (GTEST_SRC_DIR)

    endif (GTEST_INCLUDE_DIR)

endif (NOT GTEST_FOUND)

# If we still couldn't find it, then set up an external project
# and build it there
macro (yiqi_import_library_from_extproject library_target location extproj)

    add_library (${library_target} STATIC IMPORTED GLOBAL)
    set_target_properties (${library_target}
                           PROPERTIES IMPORTED_LOCATION ${location})
    add_dependencies (${library_target} ${extproj})

endmacro (yiqi_import_library_from_extproject)

if (NOT GTEST_FOUND)

    include (ExternalProject)

    set (BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})

    set (GMOCK_EXT_PROJECT_NAME GoogleMock)
    set (GMOCK_PREFIX ${BIN_DIR}/__gmock)
    set (GMOCK_SOURCE_DIR ${GMOCK_PREFIX}/src/${GMOCK_EXT_PROJECT_NAME})
    set (GMOCK_DEFAULT_BINARY_DIR ${GMOCK_SOURCE_DIR}-build)
    set (GTEST_DEFAULT_BINARY_DIR ${GMOCK_DEFAULT_BINARY_DIR}/gtest)
    set (GTEST_SOURCE_DIR ${GMOCK_SOURCE_DIR}/gtest)
    set (GMOCK_URL "http://googlemock.googlecode.com/files/gmock-1.7.0.zip")

    set (EXTPROJECT_TARGET GoogleMock)

    ExternalProject_Add (${EXTPROJECT_TARGET}
                         URL ${GMOCK_URL}
                         PREFIX ${GMOCK_PREFIX}
                         INSTALL_COMMAND "")

    set (GMOCK_LIBRARY_PATH ${GMOCK_DEFAULT_BINARY_DIR}/libgmock.a)
    set (GMOCK_MAIN_LIBRARY_PATH ${GMOCK_DEFAULT_BINARY_DIR}/libgmock_main.a)
    set (GTEST_LIBRARY_PATH ${GTEST_DEFAULT_BINARY_DIR}/libgtest.a)
    set (GTEST_MAIN_LIBRARY_PATH ${GTEST_DEFAULT_BINARY_DIR}/libgtest_main.a)

    set (GMOCK_LIBRARY gmock)
    set (GMOCK_MAIN_LIBRARY gmock_main)
    set (GTEST_LIBRARY gtest)
    set (GTEST_MAIN_LIBRARY gtest_main)
    set (GTEST_BOTH_LIBRARIES
         ${CMAKE_THREAD_LIBS_INIT}
         ${GTEST_LIBRARY}
         ${GTEST_MAIN_LIBRARY})

    set (GTEST_INCLUDE_DIR ${GTEST_SOURCE_DIR}/include)
    set (GMOCK_INCLUDE_DIR ${GMOCK_SOURCE_DIR}/include)

    # Tell CMake that we've imported some libraries
    yiqi_import_library_from_extproject (${GMOCK_LIBRARY}
                                         ${GMOCK_LIBRARY_PATH}
                                         ${EXTPROJECT_TARGET})
    yiqi_import_library_from_extproject (${GMOCK_MAIN_LIBRARY}
                                         ${GMOCK_MAIN_LIBRARY_PATH}
                                         ${EXTPROJECT_TARGET})
    yiqi_import_library_from_extproject (${GTEST_LIBRARY}
                                         ${GTEST_LIBRARY_PATH}
                                         ${EXTPROJECT_TARGET})
    yiqi_import_library_from_extproject (${GTEST_MAIN_LIBRARY}
                                         ${GTEST_MAIN_LIBRARY_PATH}
                                         ${EXTPROJECT_TARGET})

    set (GTEST_FOUND 1)

endif (NOT GTEST_FOUND)

if (NOT GTEST_FOUND)
    message (SEND_ERROR "Google Test and Google Mock "
                        "are required to build Yiqi")
endif (NOT GTEST_FOUND)

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
     ${GMOCK_INCLUDE_DIR})
set (YIQI_EXTERNAL_LIBRARY_DIRS
     ${YIQI_VALGRIND_LIBRARY_DIRS}
     ${Boost_LIBRARY_DIRS})
set (YIQI_EXTERNAL_LIBRARIES
     ${Boost_LIBRARIES})
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
