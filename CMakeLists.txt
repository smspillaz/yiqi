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
set (CMAKE_MODULE_PATH
     ${YIQI_VERAPP_CMAKE_DIRECTORY} ${CMAKE_MODULE_PATH})

set (Boost_USE_STATIC_LIBS ON)

find_package (Boost 1.46 COMPONENTS program_options iostreams)
find_package (GTest QUIET)
find_package (VeraPP 1.2 REQUIRED)

include (FindPkgConfig)
pkg_check_modules (YIQI_VALGRIND REQUIRED valgrind)

# We must try to find google-mock first, if that is unavailable then
# building it will build both it and google-test
find_library (GMOCK_LIBRARY gmock)
find_library (GMOCK_MAIN_LIBRARY gmock_main)

if (NOT GMOCK_LIBRARY OR NOT GMOCK_MAIN_LIBRARY)

    set (GMOCK_INCLUDE_BASE "include/")

    # Find the Google Mock include directory by
    # searching the system paths
    find_path (GMOCK_INCLUDE_DIR
               gmock/gmock.h)

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

    set (GMOCK_SRC_DIR ${GMOCK_INCLUDE_PREFIX}/src/gmock)
    set (GMOCK_INCLUDE_DIR ${GMOCK_INCLUDE_DIR})
    set (GTEST_INCLUDE_DIR ${GMOCK_SRC_DIR}/gtest/include)

    add_subdirectory (${GMOCK_SRC_DIR}
                      ${CMAKE_CURRENT_BINARY_DIR}/src/gmock)

    set (GMOCK_LIBRARY gmock)
    set (GMOCK_MAIN_LIBRARY gmock_main)
    set (GTEST_LIBRARY gtest)
    set (GTEST_MAIN_LIBRARY gmock_main)
    set (GTEST_BOTH_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} gtest gtest_main)
    set (GTEST_FOUND TRUE)

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

        set (GTEST_SRC_DIR ${GTEST_INCLUDE_PREFIX}/src/gtest)

        add_subdirectory (${GTEST_SRC_DIR}
                          ${CMAKE_CURRENT_BINARY_DIR}/src/gtest)

        set (GTEST_LIBRARY gtest)
        set (GTEST_MAIN_LIBRARY gtest_main)
        set (GTEST_BOTH_LIBRARIES
             ${GTEST_LIBRARY}
             ${GTEST_MAIN_LIBRARY})

        set (GTEST_FOUND TRUE)
    endif (GTEST_INCLUDE_DIR)

endif (NOT GTEST_FOUND)

if (NOT GTEST_FOUND OR NOT GMOCK_LIBRARY OR NOT GMOCK_MAIN_LIBRARY)
    message (SEND_ERROR "Google Test is required to build Yiqi")
endif (NOT GTEST_FOUND OR NOT GMOCK_LIBRARY OR NOT GMOCK_MAIN_LIBRARY)

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

# Set up Folly
find_path (FOLLY_INCLUDE_DIR
           folly/folly-config.h)

find_library (FOLLY_LIBRARY folly)
find_library (FOLLY_BENCHMARK_LIBRARY follybenchmark)
find_library (FOLLY_FINGERPRINT_LIBRARY follyfingerprint)
find_library (FOLLY_TIMEOUT_QUEUE_LIBRARY follytimeout_queue)

if (NOT FOLLY_INCLUDE_DIR OR
    NOT FOLLY_LIBRARY OR
    NOT FOLLY_BENCHMARK_LIBRARY OR
    NOT FOLLY_FINGERPRINT_LIBRARY OR
    NOT FOLLY_TIMEOUT_QUEUE_LIBRARY)
    message (SEND_ERROR "Folly (http://github.com/facebook/folly) is"
                        " required to build yiqi")
endif (NOT FOLLY_INCLUDE_DIR OR
       NOT FOLLY_LIBRARY OR
       NOT FOLLY_BENCHMARK_LIBRARY OR
       NOT FOLLY_FINGERPRINT_LIBRARY OR
       NOT FOLLY_TIMEOUT_QUEUE_LIBRARY)

# -fPIC, -Wall and -Werror are mandatory
set (COMPILER_FLAGS "-fPIC -Wall -Werror")
set (CXX_CXX11_FLAGS "-std=c++0x")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} ${CXX_CXX11_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}")

set (YIQI_EXTERNAL_INCLUDE_DIRS
     ${YIQI_VALGRIND_INCLUDE_DIRS}
     ${Boost_INCLUDE_DIRS}
     ${FOLLY_INCLUDE_DIR}
     ${GTEST_INCLUDE_DIR}
     ${GMOCK_INCLUDE_DIR})
set (YIQI_EXTERNAL_LIBRARY_DIRS
     ${YIQI_VALGRIND_LIBRARY_DIRS}
     ${Boost_LIBRARY_DIRS})
set (YIQI_EXTERNAL_LIBRARIES
     ${Boost_LIBRARIES})
set (YIQI_INTERNAL_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set (YIQI_INTERNAL_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (YIQI_SAMPLES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sample)
set (YIQI_TESTS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set (YIQI_MOCKS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
set (YIQI_MATCHERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/matchers)
set (YIQI_TESTS_UTIL_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testsutil)

set (YIQI_MOCKS_LIBRARY yiqi_mocks)
set (YIQI_MATCHERS_LIBRARY yiqi_matchers)
set (YIQI_TESTS_UTIL_LIBRARY yiqi_tests_util)

set (YIQI_MAIN_LIBRARY yiqi_main)
set (YIQI_LIBRARY yiqi)

add_subdirectory (${YIQI_INTERNAL_INCLUDE_DIRECTORY})
add_subdirectory (${YIQI_INTERNAL_SOURCE_DIRECTORY})
add_subdirectory (${YIQI_SAMPLES_DIRECTORY})
add_subdirectory (${YIQI_TESTS_DIRECTORY})
add_subdirectory (${YIQI_MOCKS_DIRECTORY})
add_subdirectory (${YIQI_MATCHERS_DIRECTORY})
add_subdirectory (${YIQI_TESTS_UTIL_DIRECTORY})
