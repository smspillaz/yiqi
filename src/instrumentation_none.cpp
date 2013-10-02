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
namespace yitp = yiqi::instrumentation::tools::programs;

namespace
{
    class NoneProgram :
        public yit::Program
    {
        private:

            std::string const & WrapperBinary () const;
            std::string const & WrapperOptions () const;
            std::string const & Name () const;
            yconst::InstrumentationTool ToolIdentifier () const;
    };
}

std::string const &
NoneProgram::Name () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
}

yconst::InstrumentationTool
NoneProgram::ToolIdentifier () const
{
    return yconst::InstrumentationTool::None;
}

std::string const &
NoneProgram::WrapperBinary () const
{
    static std::string const wrapper ("");
    return wrapper;
}

std::string const &
NoneProgram::WrapperOptions () const
{
    static std::string const options ("");
    return options;
}

yitp::Unique
yitp::MakeNone ()
{
    return yitp::Unique (new NoneProgram ());
}
