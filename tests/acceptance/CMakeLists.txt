# /tests/acceptance/CMakeLists.txt
# Build Yiqi Acceptance Tests. This includes some utility helper binaries to
# simulate the instrumentation process, (eg, a simple pass-through) as well
# as a test binary that is linked to yiqi_main so that we run our system
# as a whole externally.
#
# Note that gtest_main is linked in here instead of yiqi_main, as we don't
# want to depend on a running instance of an instrumentation program

set (YIQI_PASSTHROUGH_SUBDIRECTORY passthrough)

set (YIQI_PASSTHROUGH_EXEC_SOURCE_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/${YIQI_PASSTHROUGH_SUBDIRECTORY})
set (YIQI_PASSTHROUGH_EXEC_BINARY_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/${YIQI_PASSTHROUGH_SUBDIRECTORY})

set (YIQI_PASSTHROUGH_EXEC passthrough)
set (YIQI_PASSTHROUGH_CONSTANTS_LIBRARY passthrough_constants)

set (YIQI_PASSTHROUGH_EXEC_BINARY_LOCATION
    ${YIQI_PASSTHROUGH_EXEC_BINARY_DIRECTORY}/${YIQI_PASSTHROUGH_EXEC})

add_subdirectory (${YIQI_PASSTHROUGH_EXEC_SOURCE_DIRECTORY})

set (YIQI_SIMPLE_TEST_SUBDIRECTORY simple_test_binary)
set (YIQI_SIMPLE_TEST_SOURCE_DIRECTORY
     ${CMAKE_CURRENT_SOURCE_DIR}/${YIQI_SIMPLE_TEST_SUBDIRECTORY})
set (YIQI_SIMPLE_TEST_BINARY_DIRECTORY
     ${CMAKE_CURRENT_BINARY_DIR}/${YIQI_SIMPLE_TEST_SUBDIRECTORY})

set (YIQI_SIMPLE_TEST_EXEC simple_test)

set (YIQI_SIMPLE_TEST_EXEC_BINARY_LOCATION
    ${YIQI_SIMPLE_TEST_BINARY_DIRECTORY}/${YIQI_SIMPLE_TEST_EXEC})

add_subdirectory (${YIQI_SIMPLE_TEST_SOURCE_DIRECTORY})

set (YIQI_ACCEPTANCE_TESTS_CONFIG_FILE_H_INPUT
     ${CMAKE_CURRENT_SOURCE_DIR}/acceptance_tests_config.h.in)
set (YIQI_ACCEPTANCE_TESTS_CONFIG_FILE_H_OUTPUT
     ${CMAKE_CURRENT_BINARY_DIR}/acceptance_tests_config.h)

# We can't use configure_file here since we need to know the
# location of the binaries at build time and not configure time
# because of the way some build systems work (Xcode, VS, etc)
set (REPLACEMENT_GENERATOR
     ${YIQI_SIMPLE_STRING_REPLACEMENT_GENERATOR})

add_custom_command (OUTPUT ${YIQI_ACCEPTANCE_TESTS_CONFIG_FILE_H_OUTPUT}
                    COMMAND $<TARGET_FILE:${REPLACEMENT_GENERATOR}>
                    ARGS
                    -i ${YIQI_ACCEPTANCE_TESTS_CONFIG_FILE_H_INPUT}
                    -o ${YIQI_ACCEPTANCE_TESTS_CONFIG_FILE_H_OUTPUT}
                    YIQI_SIMPLE_TEST_EXEC_BINARY_LOCATION
                    $<TARGET_FILE:${YIQI_SIMPLE_TEST_EXEC}>
                    YIQI_PASSTHROUGH_EXEC_BINARY_LOCATION
                    $<TARGET_FILE:${YIQI_PASSTHROUGH_EXEC}>
                    YIQI_PASSTHROUGH_EXEC_BINARY_DIRECTORY
                    $<TARGET_FILE_DIR:${YIQI_PASSTHROUGH_EXEC}>
                    DEPENDS
                    ${REPLACEMENT_GENERATOR}
                    ${YIQI_ACCEPTANCE_TESTS_CONFIG_FILE_H_INPUT}
                    COMMENT "Generating acceptance tests configuration")

include_directories (${YIQI_PASSTHROUGH_EXEC_SOURCE_DIRECTORY}
                     ${YIQI_INTERNAL_INCLUDE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                     ${YIQI_INTERNAL_SOURCE_BINARY_DIRECTORY}
                     ${YIQI_MOCKS_DIRECTORY}
                     ${YIQI_MATCHERS_DIRECTORY}
                     ${YIQI_TESTS_UTIL_DIRECTORY}
                     ${YIQI_EXTERNAL_INCLUDE_DIRS}
                     ${CMAKE_CURRENT_BINARY_DIR})

set (YIQI_ACCEPTANCE_TESTS_BINARY yiqi_acceptance_tests)
set (YIQI_ACCEPTANCE_TESTS_SRCS
     ${CMAKE_CURRENT_SOURCE_DIR}/reexecute_through_passthrough.cpp)

# Slight workaround for the fact that we can't include the config file output
# in style checks as it may contain lines which are quite long
set_property (SOURCE ${YIQI_ACCEPTANCE_TESTS_SRCS}
              PROPERTY OBJECT_DEPENDS
              ${YIQI_ACCEPTANCE_TESTS_CONFIG_FILE_H_OUTPUT})

add_executable (${YIQI_ACCEPTANCE_TESTS_BINARY}
                ${YIQI_ACCEPTANCE_TESTS_SRCS})

verapp_profile_check_source_files_conformance (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                               ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${YIQI_VERAPP_PROFILE}
                                               ${YIQI_ACCEPTANCE_TESTS_BINARY}
                                               ${YIQI_VERAPP_IMPORT_RULES}
                                               ERROR)

cppcheck_target_sources (${YIQI_ACCEPTANCE_TESTS_BINARY}
                         INCLUDES
                         ${YIQI_INTERNAL_SOURCE_DIRECTORY}
                         ${YIQI_INTERNAL_INCLUDE_DIRECTORY})

target_link_libraries (${YIQI_ACCEPTANCE_TESTS_BINARY}
                       ${YIQI_PASSTHROUGH_CONSTANTS_LIBRARY}
                       ${YIQI_LIBRARY}
                       ${YIQI_MOCKS_LIBRARY}
                       ${YIQI_MATCHERS_LIBRARY}
                       ${YIQI_TESTS_UTIL_LIBRARY}
                       ${GTEST_LIBRARY}
                       ${GMOCK_LIBRARY}
                       ${YIQI_TESTS_MAIN_LIBRARY})
