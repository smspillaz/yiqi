/*
 * instrumentation_cachegrind.h:
 * Provides an implementation of a yiqi::instrumentation::tools::Tool
 * which profiles the code under test using cachegrind
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

namespace
{
    class CachegrindProgram :
        public yitv::ProgramBase
    {
        private:

            Program::ToolID ToolIdentifier () const;
            std::string const & ToolAdditionalOptions () const;
    };
}

yconst::InstrumentationTool
CachegrindProgram::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Cachegrind;
}

std::string const &
CachegrindProgram::ToolAdditionalOptions () const
{
    static std::string const options ("");
    return options;
}

yitp::Unique
yitp::MakeCachegrind ()
{
    return yitp::Unique (new CachegrindProgram ());
}
