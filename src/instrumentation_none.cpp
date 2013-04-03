/*
 * instrumentation_none.h:
 * Provides an implementation of a yiqi::instrumentation::tools::Tool
 * which does not perform any instrumentation at all
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
    class NoneTool :
        public yit::Tool
    {
        private:

            std::string const & InstrumentationWrapper () const;
            std::string const & WrapperOptions () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };
}

yconst::InstrumentationTool
NoneTool::ToolIdentifier () const
{
    return yconst::InstrumentationTool::None;
}

std::string const &
NoneTool::InstrumentationWrapper () const
{
    static std::string const wrapper ("");
    return wrapper;
}

std::string const &
NoneTool::WrapperOptions () const
{
    static std::string const options ("");
    return options;
}

yit::ToolUniquePtr
yit::MakeNoneTool ()
{
    return yit::ToolUniquePtr (new NoneTool ());
}
