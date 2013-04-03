/*
 * instrumentation_callgrind.h:
 * Provides an implementation of a yiqi::instrumentation::tools::Tool
 * which profiles the coder under test using callgrind
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
    class CallgrindTool :
            public yitv::ToolBase
    {
        private:

            Tool::ToolID ToolIdentifier () const;
            std::string const & ToolAdditionalOptions () const;
    };
}

yconst::InstrumentationTool
CallgrindTool::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Callgrind;
}

std::string const &
CallgrindTool::ToolAdditionalOptions () const
{
    static std::string const options ("");
    return options;
}

yit::ToolUniquePtr
yit::MakeCallgrindTool ()
{
    return yit::ToolUniquePtr (new CallgrindTool ());
}
