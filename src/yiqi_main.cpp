/*
 * yiqi_main.cpp:
 * Provide the main () function for the tests, parse options,
 * inject environment and re-exec () as necessary.
 *
 * See LICENCE.md for Copyright information
 */
#include <gtest/gtest.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <unistd.h>

#include "commandline.h"
#include "construction.h"
#include "instrumentation_tool.h"
#include "systempaths.h"
#include "system_api.h"
#include "system_implementation.h"

namespace po = boost::program_options;
namespace ycom = yiqi::commandline;
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

    po::options_description desc (yc::FetchOptionsDescription ());

    /* Figure out if we need to re-exec here under valgrind */
    yit::Tool::Unique tool (yc::ParseOptionsToToolUniquePtr (argc,
                                                             argv,
                                                             desc));

    ycom::CommandArguments args (ycom::BuildCommandLine (argc,
                                                         argv,
                                                         *tool));
    ycom::ArgvVector       argvVec (ycom::StringVectorToArgv (args));

    return RUN_ALL_TESTS ();
}
