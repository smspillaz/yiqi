/*
 * construction.cpp:
 * Implementation of the options -> construction parameters logic
 *
 * See LICENCE.md for Copyright information
 */

#include "construction.h"
#include "constants.h"
#include "instrumentation_tool.h"

namespace yconst = yiqi::constants;
namespace yc = yiqi::construction;
namespace yit = yiqi::instrumentation::tools;
namespace yitp = yiqi::instrumentation::tools::programs;
namespace po = boost::program_options;

namespace
{
    std::string GetNoneString ()
    {
        auto const noneTool (yconst::InstrumentationTool::None);
        auto const noneString (yconst::StringFromTool (noneTool));
        return noneString;
    }
}

po::options_description
yc::FetchOptionsDescription ()
{
    po::options_description description ("Options");
    description.add_options ()
        (yconst::YiqiToolOption,
         po::value <std::string> ()->default_value (GetNoneString ()),
         "Tool");

    return description;
}

std::string
yc::ParseOptionsForToolName (int                argc,
                             const char * const *argv,
                             const yc::Options  &description)
{
    po::variables_map       variableMap;
    po::command_line_parser parser (argc, argv);

    po::store (parser.options (description).run (),
               variableMap);
    po::notify (variableMap);

    if (variableMap.count (yconst::YiqiToolOption))
        return variableMap[yconst::YiqiToolOption].as <std::string> ();

    return GetNoneString ();
}

yitp::Unique
yc::MakeProgramInfo (yconst::InstrumentationTool toolID)
{
    typedef yitp::Unique (*ToolFactory) ();
    typedef std::map <yconst::InstrumentationTool, ToolFactory> FactoryMap;

    static FactoryMap const toolConstructors
    {
        { yconst::InstrumentationTool::None, yitp::MakeNone },
        { yconst::InstrumentationTool::Timer, yitp::MakeTimer },
        { yconst::InstrumentationTool::Memcheck, yitp::MakeMemcheck },
        { yconst::InstrumentationTool::Callgrind, yitp::MakeCallgrind },
        { yconst::InstrumentationTool::Cachegrind, yitp::MakeCachegrind },
        { yconst::InstrumentationTool::Passthrough, yitp::MakePassthrough },
    };

    ToolFactory const factory = toolConstructors.at (toolID);
    return factory ();
}
