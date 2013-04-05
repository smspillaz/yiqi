# /CMakeLists.txt
# Main CMake file for the Yiqi project. Finds Google Test
# and determines if we can build Yiqi
#
# See LICENCE.md for Copyright information

project (YiqiInstrumentation)

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

include (FindPkgConfig)

set (Boost_USE_STATIC_LIBS ON)

pkg_check_modules (YIQI_VALGRIND REQUIRED valgrind)
find_package (Boost 1.49 COMPONENTS program_options)
find_package (GTest QUIET)

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

find_library (GMOCK_LIBRARY gmock)
find_library (GMOCK_MAIN_LIBRARY gmock_main)

if (NOT GTEST_FOUND OR NOT GMOCK_LIBRARY OR NOT GMOCK_MAIN_LIBRARY)
    message (SEND_ERROR "Google Test is required to build Yiqi")
endif (NOT GTEST_FOUND OR NOT GMOCK_LIBRARY OR NOT GMOCK_MAIN_LIBRARY)

# -fPIC, -Wall and -Werror are mandatory
set (COMPILER_FLAGS "-fPIC -Wall -Werror")
set (CXX_CXX11_FLAGS "-std=c++11")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS} ${CXX_CXX11_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}")

set (YIQI_EXTERNAL_INCLUDE_DIRS
     ${YIQI_VALGRIND_INCLUDE_DIRS}
     ${Boost_INCLUDE_DIRS})
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

set (YIQI_MOCKS_LIBRARY yiqi_mocks)
set (YIQI_MATCHERS_LIBRARY yiqi_matchers)

set (YIQI_MAIN_LIBRARY yiqi_main)
set (YIQI_LIBRARY yiqi)

add_subdirectory (${YIQI_INTERNAL_SOURCE_DIRECTORY})
add_subdirectory (${YIQI_SAMPLES_DIRECTORY})
add_subdirectory (${YIQI_TESTS_DIRECTORY})
add_subdirectory (${YIQI_MOCKS_DIRECTORY})
add_subdirectory (${YIQI_MATCHERS_DIRECTORY})
