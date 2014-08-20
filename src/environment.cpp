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

#include <yiqi/init.h>

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
    class YiqiToolDispatch :
        public ::testing::EmptyTestEventListener
    {
        public:

            YiqiToolDispatch (int argc, char **argv);
            virtual ~YiqiToolDispatch () {};
            virtual void SetUp ();

        private:

            class Private;
            std::unique_ptr <Private> priv;
    };

    class YiqiToolDispatch::Private
    {
        public:

            Private (int argc, char **argv);

            int argc;
            char **argv;

            std::unique_ptr <yit::Controller> controller;
    };
}

YiqiToolDispatch::Private::Private (int argc,
                                    char **argv) :
    argc (argc),
    argv (argv)
{
}

YiqiToolDispatch::YiqiToolDispatch (int argc,
                                    char **argv) :
    priv (new Private (argc, argv))
{
}

void
// cppcheck-suppress unusedFunction
YiqiToolDispatch::SetUp ()
{
}
