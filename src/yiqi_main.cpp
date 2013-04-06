/*
 * yiqi_main.cpp:
 * Provide the main () function for the tests, parse options,
 * inject environment and re-exec () as necessary.
 *
 * See LICENCE.md for Copyright information
 */
#include <gtest/gtest.h>

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <unistd.h>

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
namespace ysys = yiqi::system;
namespace ysysapi = yiqi::system::api;

namespace
{
    class YiqiEnvironment :
        public ::testing::Environment
    {
        public:

            virtual ~YiqiEnvironment () = default;
            virtual void SetUp ();
            virtual void TearDown ();
    };
}

void
YiqiEnvironment::SetUp ()
{
}

void
YiqiEnvironment::TearDown()
{
}

int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    ::testing::AddGlobalTestEnvironment(new YiqiEnvironment);

    char const *activeTool = getenv (yconst::YiqiToolEnvKey);

    if (activeTool)
    {
	std::cout << yconst::YiqiRunningUnderHeader
                  << std::string (activeTool)
                  << std::endl;
    }
    else
    {
        po::options_description desc (yc::FetchOptionsDescription ());

        /* Figure out if we need to re-exec here under valgrind */
        yit::Tool::Unique tool (yc::ParseOptionsToToolUniquePtr (argc,
                                                                 argv,
                                                                 desc));

        /* We can skip a bit if there is no instrumentation wrapper */
        if (!tool->InstrumentationWrapper ().empty ())
        {
            ysysapi::SystemCalls::Unique calls (ysysapi::MakeUNIXSystemCalls ());

            yexec::RelaunchCurrentProgram (*tool,
                                           argc,
                                           argv,
                                           *calls);
        }
    }

    return RUN_ALL_TESTS ();
}
