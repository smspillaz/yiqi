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

namespace yiqi
{
    namespace constants
    {
        /**
         * @brief YiqiToolOption
         * @return the current string describing how to specify
         * the instrumentation tool on the command line
         */
        char const * YiqiToolOption ();

        /**
         * @brief The InstrumentationTools enum lists
         * all of the available tools that we can use
         * for instrumentation. Use it to look up data
         * relating to these tools in other stored vectors
         * etc
         */
        enum class InstrumentationTool
        {
            None = 0,
            Timer = 1,
            Memcheck = 2,
            Callgrind = 3,
            Cachegrind = 4
        };

        struct InstrumentationToolName
        {
            InstrumentationTool tool;
            char          const *name;
        };

        /**
         * @brief InstrumentationToolNames
         * @return an array of all instrumentation tool names
         * arranged by the order specified in InstrumentationTools
         */
        InstrumentationToolName const * InstrumentationToolNames ();
    }
}

#endif
