# /tests/CMakeLists.txt
# Builds the different kinds of tests available for Yiqi
#
# See LICENCE.md for Copyright information

set (YIQI_UNIT_TESTS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/unit)
set (YIQI_INTEGRATION_TESTS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/integration)
set (YIQI_ACCEPTANCE_TESTS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/acceptance)

add_subdirectory (${YIQI_UNIT_TESTS_DIRECTORY})
add_subdirectory (${YIQI_INTEGRATION_TESTS_DIRECTORY})
add_subdirectory (${YIQI_ACCEPTANCE_TESTS_DIRECTORY})
