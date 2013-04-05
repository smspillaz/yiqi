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

char const *
yconst::ValgrindWrapper ()
{
    static char const *valgrind = "valgrind";
    return valgrind;
}

char const *
yconst::ValgrindToolOptionPrefix ()
{
    static char const *prefix = "--tool=";
    return prefix;
}

char const *
yconst::YiqiToolOption ()
{
    static char const *tool = "yiqi_tool";
    return tool;
}

char const *
yconst::YiqiToolEnvKey ()
{
    static char const *key = "__YIQI_INSTRUMENTATION_TOOL_ACTIVE";
    return key;
}

yconst::ToolsArray const & yconst::InstrumentationToolNames()
{
    static ToolsArray const names =
    {
        {
            { InstrumentationTool::None, "none" },
            { InstrumentationTool::Timer, "timer" },
            { InstrumentationTool::Memcheck, "memcheck" },
            { InstrumentationTool::Callgrind, "callgrind" },
            { InstrumentationTool::Cachegrind, "cachegrind" }
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
        for (auto const &tool : yconst::InstrumentationToolNames ())
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
        for (auto const &tool : yconst::InstrumentationToolNames ())
            stringToToolMap[tool.name] = tool.tool;
    });

    return stringToToolMap[str];
}
