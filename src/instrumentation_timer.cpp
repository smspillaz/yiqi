/*
 * instrumentation_timer.h:
 * Provides an implementation of a yiqi::instrumentation::tools::Tool
 * which would provide timing data
 *
 * See LICENCE.md for Copyright information
 */

#include "constants.h"
#include "instrumentation_tool.h"
#include "instrumentation_tools_available.h"

namespace yconst = yiqi::constants;
namespace yit = yiqi::instrumentation::tools;

namespace
{
    class TimerTool :
        public yit::Tool
    {
        private:

            std::string const & InstrumentationWrapper () const;
            std::string const & WrapperOptions () const;
            std::string const & InstrumentationName () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };
}

yconst::InstrumentationTool
TimerTool::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Timer;
}

std::string const &
TimerTool::InstrumentationName () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
}

std::string const &
TimerTool::InstrumentationWrapper () const
{
    static std::string const wrapper ("");
    return wrapper;
}

std::string const &
TimerTool::WrapperOptions () const
{
    static std::string const options ("");
    return options;
}

yit::ToolUniquePtr
yit::MakeTimerTool ()
{
    return yit::ToolUniquePtr (new TimerTool ());
}
