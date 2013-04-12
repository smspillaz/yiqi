# VeraPPTargets.cmake
# Set up some of the main targets that we will use in order
# to determine when the Vera++ rules need to be imported
#
# See LICENCE.md for Copyright information

include (VeraPPUtilities)

add_custom_target (${YIQI_VERAPP_IMPORT_RULES} ALL)

set (_import_target ${YIQI_VERAPP_IMPORT_RULES})
set (_rules_out_dir ${YIQI_VERAPP_RULES_OUTPUT_DIRECTORY})
set (_profiles_out_dir ${YIQI_VERAPP_PROFILES_OUTPUT_DIRECTORY})
set (_rules_in_dir ${YIQI_VERAPP_RULES_SOURCE_DIRECTORY})
set (_profiles_in_dir ${YIQI_VERAPP_PROFILES_SOURCE_DIRECTORY})

verapp_import_default_rules_into_subdirectory_on_target (${_rules_out_dir}
                                                         ${_import_target})

verapp_copy_files_in_dir_to_subdir_on_target (${_rules_in_dir}
                                              ${_rules_out_dir}
                                              .tcl
                                              yiqi_verapp_copy_rules_files
                                              "Yiqi Vera++ rule")

add_dependencies (${_import_target} yiqi_verapp_copy_rules_files)

verapp_copy_files_in_dir_to_subdir_on_target (${_profiles_in_dir}
                                              ${_profiles_out_dir}
                                              NO_MATCH
                                              yiqi_verapp_copy_profile_files
                                              "Yiqi Vera++ profile")

add_dependencies (${_import_target} yiqi_verapp_copy_profile_files)
