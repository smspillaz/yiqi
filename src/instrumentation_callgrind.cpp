/*
 * instrumentation_callgrind.h:
 * Provides an implementation of a yiqi::instrumentation::tools::Tool
 * which profiles the coder under test using callgrind
 *
 * See LICENCE.md for Copyright information
 */

#include <sstream>
#include <mutex>

#include <valgrind.h>

#include "constants.h"
#include "instrumentation_tool.h"
#include "instrumentation_tool_valgrind_base.h"
#include "instrumentation_tools_available.h"

namespace yconst = yiqi::constants;
namespace yit = yiqi::instrumentation::tools;
namespace yitv = yiqi::instrumentation::tools::valgrind;
namespace yitp = yiqi::instrumentation::tools::programs;
namespace yitc = yiqi::instrumentation::tools::controllers;

namespace
{
    class CallgrindProgram :
        public yitv::ProgramBase
    {
        private:

            Program::ToolID ToolIdentifier () const;
            std::string const & ToolAdditionalOptions () const;
    };

    class CallgrindController :
        public yit::Controller
    {
        private:

            Controller::ToolID ToolIdentifier () const;
            void Start ();
            void Stop (FinishMode mode);
    };
}

yconst::InstrumentationTool
CallgrindProgram::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Callgrind;
}

std::string const &
CallgrindProgram::ToolAdditionalOptions () const
{
    static std::string const options ("");
    return options;
}

void
CallgrindController::Start ()
{
}

void
CallgrindController::Stop (FinishMode mode)
{
}

yconst::InstrumentationTool
CallgrindController::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Callgrind;
}

yitp::Unique
yitp::MakeCallgrind ()
{
    return yitp::Unique (new CallgrindProgram ());
}

yitc::Unique
yitc::MakeCallgrind ()
{
    return yitc::Unique (new CallgrindController ());
}
