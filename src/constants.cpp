/*
 * constants.cpp:
 * Provide some definitions for constants which
 * will be used throughout the framework
 *
 * See LICENCE.md for Copyright information
 */

#include <algorithm>
#include <cstring>
#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include "constants.h"
#include "instrumentation_tools_available.h"

namespace yconst = yiqi::constants;

char const * yconst::ValgrindWrapper = "valgrind";
char const * yconst::ValgrindToolOptionPrefix = "--tool=";
char const * yconst::YiqiToolOption = "yiqi_tool";
char const * yconst::YiqiToolEnvKey = "__YIQI_INSTRUMENTATION_TOOL_ACTIVE";
char const * yconst::YiqiRunningUnderHeader =
    "[YIQI] RUNNING UNDER INSTRUMENTATION: ";

char const *
yconst::StringFromTool (InstrumentationTool toolValue)
{
    unsigned int index = static_cast <unsigned int> (toolValue);
    return yconst::InstrumentationToolNames ()[index].name;

}

yconst::InstrumentationTool
yconst::ToolFromString (const std::string &str)
{
    yconst::ToolsArray const &array (yconst::InstrumentationToolNames ());
    auto match =
        [](InstrumentationToolName const &tool, std::string const &s) -> bool {
            return std::strcmp (tool.name, s.c_str ()) < 0;
        };
    auto it (std::lower_bound (array.begin (),
                               array.end (),
                               str,
                               match));
    if (it != array.end () &&
        std::strcmp (it->name, str.c_str ()) == 0)
        return it->tool;

    std::stringstream ss;
    ss << "No such tool with name " << str;

    throw std::logic_error (ss.str ());
}
