/*
 * test_util.h:
 * Utility functions and wrappers shared amonst testing
 * translation units
 *
 * See LICENCE.md for Copyright information
 */

#include "test_util.h"

namespace ytest = yiqi::testing;

char const *ytest::MockProgramName = "mock_program_name";

int
ytest::ArgumentCount (CommandLineArguments const &args)
{
    return std::get <0> (args);
}

char const * const *
ytest::Arguments (CommandLineArguments const &args)
{
    return &(std::get <1> (args))[0];
}

ytest::CommandLineArguments
ytest::GenerateCommandLine (std::vector<std::string> const &arguments)
{
    std::vector <const char *> charArguments;

    charArguments.push_back (MockProgramName);

    for (std::string const &str : arguments)
        charArguments.push_back (str.c_str ());

    return CommandLineArguments (charArguments.size (),
                                 charArguments);
}
