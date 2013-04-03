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

namespace
{
    class MemcheckTool :
            public yitv::ToolBase
    {
        private:

            Tool::ToolID ToolIdentifier () const;
            std::string const & ToolAdditionalOptions () const;
    };
}

yconst::InstrumentationTool
MemcheckTool::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Memcheck;
}

std::string const &
MemcheckTool::ToolAdditionalOptions () const
{
    static std::string const options ("");
    return options;
}

yit::ToolUniquePtr
yit::MakeMemcheckTool ()
{
    return yit::ToolUniquePtr (new MemcheckTool ());
}
