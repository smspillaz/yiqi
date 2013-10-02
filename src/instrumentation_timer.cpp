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
namespace yitp = yiqi::instrumentation::tools::programs;

namespace
{
    class TimerProgram :
        public yit::Program
    {
        private:

            std::string const & InstrumentationWrapper () const;
            std::string const & WrapperOptions () const;
            std::string const & InstrumentationName () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };
}

yconst::InstrumentationTool
TimerProgram::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Timer;
}

std::string const &
TimerProgram::InstrumentationName () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
}

std::string const &
TimerProgram::InstrumentationWrapper () const
{
    static std::string const wrapper ("");
    return wrapper;
}

std::string const &
TimerProgram::WrapperOptions () const
{
    static std::string const options ("");
    return options;
}

yitp::Unique
yitp::MakeTimer()
{
    return yitp::Unique (new TimerProgram ());
}
