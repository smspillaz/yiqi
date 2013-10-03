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
namespace yitc = yiqi::instrumentation::tools::controllers;

namespace
{
    class TimerProgram :
        public yit::Program
    {
        private:

            std::string const & WrapperBinary () const;
            std::string const & WrapperOptions () const;
            std::string const & Name () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };

    class TimerController :
        public yit::Controller
    {
        private:

            ToolID ToolIdentifier () const;
            void Start ();
            void Stop (FinishMode mode);
    };
}

yconst::InstrumentationTool
TimerProgram::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Timer;
}

std::string const &
TimerProgram::Name () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
}

std::string const &
TimerProgram::WrapperBinary () const
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

void
TimerController::Start ()
{
}

void
TimerController::Stop (FinishMode mode)
{
}

yconst::InstrumentationTool
TimerController::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Timer;
}

yitp::Unique
yitp::MakeTimer ()
{
    return yitp::Unique (new TimerProgram ());
}

yitc::Unique
yitc::MakeTimer ()
{
    return yitc::Unique (new TimerController ());
}
