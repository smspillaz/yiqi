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
namespace yitp = yiqi::instrumentation::tools::programs;

namespace
{
    class PassthroughProgram :
        public yit::Program
    {
        private:

            std::string const & WrapperBinary () const;
            std::string const & WrapperOptions () const;
            std::string const & Name () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };
}

yconst::InstrumentationTool
PassthroughProgram::ToolIdentifier () const
{
    return yconst::InstrumentationTool::Passthrough;
}

std::string const &
PassthroughProgram::WrapperBinary () const
{
    static std::string const wrapper ("passthrough");
    return wrapper;
}

std::string const &
PassthroughProgram::Name () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
}

std::string const &
PassthroughProgram::WrapperOptions () const
{
    static std::string const options ("--passthrough");
    return options;
}

yitp::Unique
yitp::MakePassthrough ()
{
    return yitp::Unique (new PassthroughProgram ());
}
