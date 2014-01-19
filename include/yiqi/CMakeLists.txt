# /include/yiqi/CMakeLists.txt
# Ensure that the yiqi headers are scanned for source code style compliance
#
# See LICENCE.md for Copyright information

set (YIQI_HEADERS
     ${CMAKE_CURRENT_SOURCE_DIR}/instrumentation.h
     ${CMAKE_CURRENT_SOURCE_DIR}/environment.h)
set (YIQI_CHECK_HEADERS_COMPLIANCE_TARGET yiqi_check_headers_compliance)

# Create a new "proxy" target and set the SOURCES property on it
# so that we can scan the source files for compliance when it is built
add_custom_target (${YIQI_CHECK_HEADERS_COMPLIANCE_TARGET} ALL
                   COMMENT "Checking headers compliance")

# Scan that proxy target for compliance
verapp_profile_check_source_files_conformance_for_target (${YIQI_VERAPP_OUTPUT_DIRECTORY}
                                                          YIQI_HEADERS
                                                          ${YIQI_VERAPP_PROFILE}
                                                          ${YIQI_CHECK_HEADERS_COMPLIANCE_TARGET}
                                                          ${YIQI_VERAPP_IMPORT_RULES}
                                                          ERROR)
