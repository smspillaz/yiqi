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

namespace
{
    class CachegrindTool :
        public yitv::ProgramBase
    {
        private:

            Program::ToolID ToolIdentifier () const;
            std::string const & ToolAdditionalOptions () const;
    };
}

yconst::InstrumentationTool
CachegrindTool::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Cachegrind;
}

std::string const &
CachegrindTool::ToolAdditionalOptions () const
{
    static std::string const options ("");
    return options;
}

yit::ToolUniquePtr
yit::MakeCachegrindTool ()
{
    return yit::ToolUniquePtr (new CachegrindTool ());
}
