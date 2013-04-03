/*
 * commandline.cpp:
 * Definitions of a function to extract a command line
 * from the currently running program argv and also the
 * instrumentation parameters
 *
 * See LICENCE.md for Copyright information
 */

#include <stdexcept>
#include "commandline.h"
#include "instrumentation_tool.h"

namespace ycom = yiqi::commandline;
namespace yit = yiqi::instrumentation::tools;

ycom::CommandArguments
ycom::BuildCommandLine (int                 argc,
                        char const * const  *argv,
                        yit::Tool const     &tool)
{
    if (argc < 1)
        throw std::runtime_error ("provided argc must have at "
                                  "least the prorgram name");

    /* In the usual case we will have three args */
    ycom::CommandArguments arguments;
    arguments.reserve (3);

    /* Instrumentation tool */
    std::string const &wrapper (tool.InstrumentationWrapper ());

    if (!wrapper.empty ())
        arguments.push_back (wrapper);

    /* The name of this test binary */
    arguments.push_back (argv[0]);

    /* Tool arguments */
    std::string const &toolOption (tool.WrapperOptions ());

    if (!toolOption.empty ())
        arguments.push_back (toolOption);

    return arguments;
}

ycom::ArgvVector const
ycom::StringVectorToArgv (CommandArguments const &args)
{
    ycom::ArgvVector argv;

    for (auto const &arg : args)
        argv.push_back (arg.c_str ());

    return argv;
}
