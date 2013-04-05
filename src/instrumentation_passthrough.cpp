/*
 * instrumentation_passthrough.cpp:
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
    class PassthroughTool :
        public yit::Tool
    {
        private:

            std::string const & InstrumentationWrapper () const;
            std::string const & WrapperOptions () const;
            std::string const & InstrumentationName () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };
}

yconst::InstrumentationTool
PassthroughTool::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Passthrough;
}

std::string const &
PassthroughTool::InstrumentationWrapper () const
{
    static std::string const wrapper ("passthrough");
    return wrapper;
}

std::string const &
PassthroughTool::InstrumentationName () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
}

std::string const &
PassthroughTool::WrapperOptions () const
{
    static std::string const options ("--passthrough");
    return options;
}

yit::ToolUniquePtr
yit::MakePassthroughTool ()
{
    return yit::ToolUniquePtr (new PassthroughTool ());
}
