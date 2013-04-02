/*
 * construction.cpp:
 * Implementation of the options -> construction parameters logic
 *
 * See LICENCE.md for Copyright information
 */

#include "construction.h"

namespace yc = yiqi::construction;
namespace po = boost::program_options;

yc::Parameters::Unique
yc::ParseOptionsToParameters (int                argc,
			      const char * const *argv,
			      const yc::Options  &description)
{
    po::variables_map optionVariableMap;

    po::store (po::command_line_parser (argc, argv)
		    .options (description).run (),
	       optionVariableMap);

    return yc::Parameters::Unique ();
}
