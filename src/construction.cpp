/*
 * construction.cpp:
 * Implementation of the options -> construction parameters logic
 *
 * See LICENCE.md for Copyright information
 */

#include "construction.h"

namespace yc = yiqi::construction;
namespace po = boost::program_options;

po::options_description
yc::FetchOptionsDescription ()
{
    po::options_description description ("Options");
    description.add_options ()
        ("yiqi_tool", po::value <std::string> ()->default_value ("none"), "Tool");

    return description;
}

std::string
yc::ParseOptionsForTool(int                argc,
                        const char * const *argv,
                        const yc::Options  &description)
{
    po::variables_map       optionVariableMap;
    po::command_line_parser parser (argc, argv);

    po::store (parser.options (description).run (),
               optionVariableMap);
    po::notify (optionVariableMap);

    if (optionVariableMap.count ("yiqi_tool"))
        return optionVariableMap["yiqi_tool"].as <std::string> ();

    return std::string ("none");
}


yc::Parameters::Unique
yc::ParseOptionsToParameters (int                argc,
                              const char * const *argv,
                              const yc::Options  &description)
{
    return yc::Parameters::Unique ();
}
