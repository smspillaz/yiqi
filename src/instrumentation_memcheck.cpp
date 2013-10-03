/*
 * instrumentation_memcheck.h:
 * Provides an implementation of a yiqi::instrumentation::tools::Tool
 * which runs the code under test through memcheck
 *
 * See LICENCE.md for Copyright information
 */

#include <sstream>
#include <mutex>

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
    class MemcheckProgram :
        public yitv::ProgramBase
    {
        private:

            Program::ToolID ToolIdentifier () const;
            std::string const & ToolAdditionalOptions () const;
    };

    class MemcheckController :
        public yit::Controller
    {
        private:

            Controller::ToolID ToolIdentifier () const;
            void Start ();
            void Stop (FinishMode mode);
    };
}

yconst::InstrumentationTool
MemcheckProgram::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Memcheck;
}

std::string const &
MemcheckProgram::ToolAdditionalOptions () const
{
    static std::string const options ("");
    return options;
}

void
MemcheckController::Start ()
{
}

void
MemcheckController::Stop (FinishMode mode)
{
}

yconst::InstrumentationTool
MemcheckController::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Memcheck;
}

yitp::Unique
yitp::MakeMemcheck ()
{
    return yitp::Unique (new MemcheckProgram ());
}

yitc::Unique
yitc::MakeMemcheck ()
{
    return yitc::Unique (new MemcheckController ());
}
