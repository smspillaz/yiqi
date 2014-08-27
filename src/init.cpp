/*
 * init.cpp:
 *
 * Global initialization routines, used to start up yiqi on a particular
 * testing platform.
 *
 * See LICENCE.md for Copyright information
 */

#include <yiqi/init.h>
#include <yiqi/platform.h>

#include "commandline.h"
#include "constants.h"
#include "construction.h"
#include "instrumentation_tool.h"
#include "reexecution.h"
#include "systempaths.h"
#include "system_api.h"
#include "system_implementation.h"

namespace po = boost::program_options;
namespace yconst = yiqi::constants;
namespace ycom = yiqi::commandline;
namespace yexec = yiqi::execution;
namespace yc = yiqi::construction;
namespace yit = yiqi::instrumentation::tools;
namespace yp = yiqi::platform;
namespace ysys = yiqi::system;
namespace ysysapi = yiqi::system::api;

namespace
{
    void InitAsInstrumented (yp::Interface &platform,
                             char const    *active)
    {
        std::cout << yconst::YiqiRunningUnderHeader
                  << std::string (active)
                  << std::endl;

        yconst::InstrumentationTool toolID (yconst::ToolFromString (active));
        yit::FactoryPackage factories (yc::FactoryPackageForTool (toolID));
        platform.Register (factories.controller ());
    }

    void InitAsParentInstance (yp::Interface &platform,
                               int           argc,
                               char          **argv)
    {
        po::options_description desc (yc::FetchOptionsDescription ());

        /* Figure out if we need to re-exec here under something */
        std::string const &toolStr (yc::ParseOptionsForToolName (argc,
                                                                 argv,
                                                                 desc));
        yconst::InstrumentationTool toolID (yconst::ToolFromString (toolStr));
        yit::FactoryPackage factories (yc::FactoryPackageForTool (toolID));

        yit::Program::Unique instrumentingProgram (factories.program ());

        /* We can skip a bit if there is no instrumentation wrapper */
        if (!instrumentingProgram->WrapperBinary ().empty ())
        {
            auto calls (ysysapi::MakeUNIXSystemCalls ());

            yexec::RelaunchCurrentProgram (*instrumentingProgram,
                                           argc,
                                          argv,
                                           *calls);
        }
        else
        {
            platform.Register (factories.controller ());
        }
    }
}

void
yiqi::Init (yp::Interface &platform,
            int           argc,
            char          **argv)
{
    char const *active = getenv (yconst::YiqiToolEnvKey);

    if (active)
        InitAsInstrumented (platform, active);
    else
        InitAsParentInstance (platform, argc, argv);
}
