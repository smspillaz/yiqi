/*
 * constants.h:
 * Provide some definitions for constants which
 * will be used throughout the framework
 *
 * See LICENCE.md for Copyright information
 */

#include "constants.h"

namespace yconst = yiqi::constants;

char const *
yconst::YiqiToolOption ()
{
    static char const *tool = "yiqi_tool";
    return tool;
}

yconst::InstrumentationToolName const *
yconst::InstrumentationToolNames ()
{
    static InstrumentationToolName const names[] =
    {
        { InstrumentationTool::None, "none" },
        { InstrumentationTool::Timer, "timer" },
        { InstrumentationTool::Memcheck, "memcheck" },
        { InstrumentationTool::Callgrind, "callgrind" },
        { InstrumentationTool::Cachegrind, "cachegrind" }
    };

    return names;
}
