/*
 * constants.h:
 * Provide some definitions for constants which
 * will be used throughout the framework
 *
 * See LICENCE.md for Copyright information
 */

#include <map>
#include <mutex>

#include "constants.h"

namespace yconst = yiqi::constants;

char const * yconst::ValgrindWrapper = "valgrind";
char const * yconst::ValgrindToolOptionPrefix = "--tool=";
char const * yconst::YiqiToolOption = "yiqi_tool";
char const * yconst::YiqiToolEnvKey = "__YIQI_INSTRUMENTATION_TOOL_ACTIVE";
char const * yconst::YiqiRunningUnderHeader = "[YIQI] RUNNING UNDER INSTRUMENTATION: ";

yconst::ToolsArray const & yconst::InstrumentationToolNames()
{
    static ToolsArray const names =
    {
        {
            { InstrumentationTool::None, "none" },
            { InstrumentationTool::Timer, "timer" },
            { InstrumentationTool::Memcheck, "memcheck" },
            { InstrumentationTool::Callgrind, "callgrind" },
            { InstrumentationTool::Cachegrind, "cachegrind" },
            { InstrumentationTool::Passthrough, "passthrough" }
        }
    };

    return names;
}

char const *
yconst::StringFromTool (InstrumentationTool toolValue)
{
    typedef std::map <InstrumentationTool, char const *> ToolToStringMap;

    static ToolToStringMap toolToStringMap;
    static std::once_flag  populateOnceFlag;

    std::call_once (populateOnceFlag, [&]() {
                        for (auto const &tool :
                             yconst::InstrumentationToolNames ())
                            toolToStringMap[tool.tool] = tool.name;
                    });

    return toolToStringMap[toolValue];

}

yconst::InstrumentationTool
yconst::ToolFromString (const std::string &str)
{
    typedef std::map <std::string, InstrumentationTool> StringToToolMap;

    static StringToToolMap stringToToolMap;
    static std::once_flag  populateOnceFlag;

    std::call_once (populateOnceFlag, [&]() {
                        for (auto const &tool :
                             yconst::InstrumentationToolNames ())
                            stringToToolMap[tool.name] = tool.tool;
                    });

    return stringToToolMap[str];
}
