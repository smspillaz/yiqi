/*
 * environment.cpp:
 *
 * Our implementation of ::testing::Environment * which
 * can be passed to ::testing::AddGlobalTestEnvironment. It is
 * responsible for setting up the tools, determining if re-execution
 * is necessary and then doing that.
 *
 * See LICENCE.md for Copyright information
 */
#include <gtest/gtest.h>

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <unistd.h>

#include <yiqi/environment.h>

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

            YiqiEnvironment (int argc, char **argv);
            virtual ~YiqiEnvironment () {};
            virtual void SetUp ();

        private:

            class Private;
            std::unique_ptr <Private> priv;
    };

    class YiqiEnvironment::Private
    {
        public:

            Private (int argc, char **argv);

            int argc;
            char **argv;
    };
}

YiqiEnvironment::Private::Private (int argc,
                                   char **argv) :
    argc (argc),
    argv (argv)
{
}

YiqiEnvironment::YiqiEnvironment (int argc,
                                  char **argv) :
    priv (new Private (argc, argv))
{
}

void
YiqiEnvironment::SetUp ()
{
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
        std::string const &toolStr (yc::ParseOptionsForToolName (priv->argc,
                                                                 priv->argv,
                                                                 desc));
        yconst::InstrumentationTool toolID (yconst::ToolFromString (toolStr));
        yit::FactoryPackage factories (yc::FactoryPackageForTool (toolID));

        yit::Program::Unique instrumentingProgram (factories.program ());

        /* We can skip a bit if there is no instrumentation wrapper */
        if (!instrumentingProgram->WrapperBinary ().empty ())
        {
            ysysapi::SystemCalls::Unique calls (ysysapi::MakeUNIXSystemCalls ());

            yexec::RelaunchCurrentProgram (*instrumentingProgram,
                                           priv->argc,
                                           priv->argv,
                                           *calls);
        }
    }
}

::testing::Environment *
yiqi::CreateTestEnvironment (int argc,
                             char **argv)
{
    return new YiqiEnvironment (argc, argv);
}
