/*
 * constants.h:
 * Provide some declarations for constants which
 * will be used throughout the framework
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_CONSTANTS_H
#define YIQI_CONSTANTS_H

#include <string>
#include <array>

namespace yiqi
{
    namespace constants
    {
        /**
         * @brief ValgrindWrapperthe binary name for valgrind
         */
        extern char const * ValgrindWrapper;

        /**
         * @brief ValgrindToolOption the prefix for specifying a valgrind tool
         */
        extern char const * ValgrindToolOptionPrefix;

        /**
         * @brief YiqiToolEnvKey the key value (e.g.
         * __YIQI_INSTRUMENTATION_TOOL_ACTIVE for the instrumented-process
         * environment)
         */
        extern char const * YiqiToolEnvKey;

        /**
         * @brief YiqiRunningUnderHeader message header when detected to be
         * running under an instrumentation tool (e.g.
         * __YIQI_INSTRUMENTATION_TOOL_ACTIVE is set)
         */
        extern char const * YiqiRunningUnderHeader;


        /**
         * @brief YiqiToolOption the current string describing how to specify
         * the instrumentation tool on the command line
         */
        extern char const * YiqiToolOption;

        /**
         * @brief The InstrumentationTools enum lists
         * all of the available tools that we can use
         * for instrumentation. Use it to look up data
         * relating to these tools in other stored vectors
         * etc
         */
        enum class InstrumentationTool;

        struct InstrumentationToolName
        {
            InstrumentationTool tool;
            char const          *name;
        };

        InstrumentationTool ToolFromString (std::string const &);
        char const *        StringFromTool (InstrumentationTool);
    }
}

#endif // YIQI_CONSTANTS_H
