/*
 * construction.cpp:
 * Implementation of the options -> construction parameters logic
 *
 * See LICENCE.md for Copyright information
 */

#include <array>
#include <sstream>
#include <string>

#include "construction.h"
#include "constants.h"
#include "instrumentation_tool.h"

namespace yconst = yiqi::constants;
namespace yc = yiqi::construction;
namespace yit = yiqi::instrumentation::tools;
namespace yitp = yiqi::instrumentation::tools::programs;
namespace yitc = yiqi::instrumentation::tools::controllers;
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

namespace
{
    class ToolNotAvailableError :
        public std::exception
    {
        public:

            ToolNotAvailableError (yconst::InstrumentationTool id);
            virtual ~ToolNotAvailableError () throw () {};
            char const * what () const throw();

        private:

            std::string msg;
    };

    /* These are sorted in the order of the enumerations in
     * yconst::InstrumentationTool */
    std::array <yit::FactoryPackage, 6> const toolConstructors =
    {
        {
            { yitp::MakeNone, yitc::MakeNone },
            { yitp::MakeTimer, yitc::MakeTimer },
            { yitp::MakeMemcheck, yitc::MakeMemcheck },
            { yitp::MakeCallgrind, yitc::MakeCallgrind },
            { yitp::MakeCachegrind, yitc::MakeCachegrind },
            { yitp::MakePassthrough, yitc::MakePassthrough }
        }
    };
}

ToolNotAvailableError::ToolNotAvailableError (yconst::InstrumentationTool id)
{
    std::stringstream ss;
    ss << "Requested unimplemented tool " << static_cast <unsigned int> (id);
    msg = ss.str();
}

char const *
ToolNotAvailableError::what () const throw()
{
    return msg.c_str ();
}

yit::FactoryPackage const &
yc::FactoryPackageForTool (yconst::InstrumentationTool toolID)
{
    try
    {
        yit::FactoryPackage const &factory =
            toolConstructors.at (static_cast <unsigned int> (toolID));
        return factory;
    }
    catch (std::range_error const &err)
    {
        throw ToolNotAvailableError (toolID);
    }
}
