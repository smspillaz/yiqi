# /cmake/ToolTracker.cmake
# A simple set of CMake functions to keep track of
# which instrumentation tools are available

set (TRACKING_PROPERTY YIQI_TOOL_TRACKER_LIST_GLOBAL_PROPERTY)

function (yiqi_add_available_tool TOOL)

    set_property (GLOBAL
                  APPEND
                  PROPERTY ${TRACKING_PROPERTY}
                  ${TOOL})

endfunction ()

function (yiqi_tools_are_available AVAILABLE)

    get_property (PROPERTY_VALUE_SET
                  GLOBAL
                  PROPERTY ${TRACKING_PROPERTY}
                  SET)

    if (PROPERTY_VALUE_SET)

        get_property (PROPERTY_VALUE
                      GLOBAL
                      PROPERTY ${TRACKING_PROPERTY})

        list (LENGTH ARGN MATCH_N)
        set (MATCHING_VARIABLES)

        foreach (TOOL_TO_MATCH ${ARGN})

            foreach (TOOL_AVAILABLE ${PROPERTY_VALUE})

                if (TOOL_TO_MATCH STREQUAL TOOL_AVAILABLE)

                    list (APPEND MATCHING_VARIABLES ${TOOL_TO_MATCH})

                endif (TOOL_TO_MATCH STREQUAL TOOL_AVAILABLE)

            endforeach ()

        endforeach ()

        if (MATCHING_VARIABLES)

            list (LENGTH MATCHING_VARIABLES MATCHING_VARIABLES_N)

        else (MATCHING_VARIABLES)

            set (MATCHING_VARIABLES_N 0)

        endif (MATCHING_VARIABLES)

        if (MATCHING_VARIABLES_N EQUAL MATCH_N)

            set (${AVAILABLE} 1 PARENT_SCOPE)

        else (MATCHING_VARIABLES_N EQUAL MATCH_N)

            set (${AVAILABLE} 0 PARENT_SCOPE)

        endif (MATCHING_VARIABLES_N EQUAL MATCH_N)

    endif (PROPERTY_VALUE_SET)

endfunction ()

function (yiqi_tools_available AVAILABLE)

    get_property (PROPERTY_VALUE_SET
                  GLOBAL
                  PROPERTY ${TRACKING_PROPERTY}
                  SET)

    if (PROPERTY_VALUE_SET)

        get_property (PROPERTY_VALUE
                      GLOBAL
                      PROPERTY ${TRACKING_PROPERTY})

        set (${AVAILABLE} ${PROPERTY_VALUE} PARENT_SCOPE)

    endif (PROPERTY_VALUE_SET)

endfunction (yiqi_tools_available)
