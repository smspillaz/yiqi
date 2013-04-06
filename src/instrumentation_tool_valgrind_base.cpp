/*
 * instrumentation_tool_valgrind_base.h:
 * Provides a template for building instrumentation tools based
 * on valgrind
 *
 * See LICENCE.md for Copyright information
 */

#include <sstream>
#include <mutex>

#include "constants.h"
#include "instrumentation_tool.h"
#include "instrumentation_tool_valgrind_base.h"

namespace yconst = yiqi::constants;
namespace yit = yiqi::instrumentation::tools;
namespace yitv = yit::valgrind;

std::string const &
yitv::ToolBase::InstrumentationWrapper () const
{
    static std::string const wrapper (yconst::ValgrindWrapper);
    return wrapper;
}

std::string const &
yitv::ToolBase::WrapperOptions () const
{
    static std::stringstream ss;
    static std::once_flag    fillStringStreamOnce;

    std::call_once (fillStringStreamOnce, [&]() {
	ss << yconst::ValgrindToolOptionPrefix
           << yconst::StringFromTool (ToolIdentifier ())
           << ToolAdditionalOptions ();
    });

    static std::string const options (ss.str ());
    return options;
}

std::string const &
yitv::ToolBase::InstrumentationName () const
{
    static std::string const name (
        yconst::StringFromTool (ToolIdentifier ()));
    return name;
}
