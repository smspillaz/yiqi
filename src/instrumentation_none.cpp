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
        public yit::Program
    {
        private:

            std::string const & InstrumentationWrapper () const;
            std::string const & WrapperOptions () const;
            std::string const & InstrumentationName () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };
}

std::string const &
NoneTool::InstrumentationName () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
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
