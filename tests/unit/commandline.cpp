/*
 * commandline.cpp:
 * Tests for building the command line
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include "commandline.h"
#include "instrumentation_tool.h"
#include "test_util.h"

#include "instrumentation_mock.h"

using ::testing::AtLeast;
using ::testing::ElementsAreArray;
using ::testing::Matcher;
using ::testing::ReturnRef;
using ::testing::StrEq;

namespace ycom = yiqi::commandline;
namespace yit = yiqi::instrumentation::tools;
namespace ymock = yiqi::mock;
namespace ymockit = yiqi::mock::instrumentation::tools;
namespace ytest = yiqi::testing;

namespace
{
    static std::string const MockWrapper ("mock");
    static std::string const MockOptions ("--mock");
    static std::string const NilString ("");
    static std::vector <std::string> const NoOptions;

    typedef ytest::CommandLineArguments CommandLineArguments;

    CommandLineArguments
    GenerateCommandLine (std::vector <std::string> const &args)
    {
        return ytest::GenerateCommandLine (args);
    }
}

class BuildCommandLine :
    public ::testing::Test
{
    public:

        BuildCommandLine () :
            instrumentation (new ymockit::Tool)
        {
            /* We don't care about whether or not these functions are
             * called, we care about how the system under test handles
             * them */
            EXPECT_CALL (*instrumentation, InstrumentationWrapper ())
                .Times (AtLeast (0));
            EXPECT_CALL (*instrumentation, WrapperOptions ())
                .Times (AtLeast (0));
        }

    protected:

        std::unique_ptr <ymockit::Tool> instrumentation;
};

TEST_F (BuildCommandLine, ThrowOnNoArgs)
{
    CommandLineArguments originalArgs;
    EXPECT_THROW ({
        ycom::BuildCommandLine (ytest::ArgumentCount (originalArgs),
                                ytest::Arguments (originalArgs),
                                *instrumentation);
    },
    std::runtime_error);
}

TEST_F (BuildCommandLine, NoWrapperOrOptionsJustArgvReturnsArgvAt0)
{
    CommandLineArguments originalArgs (GenerateCommandLine (NoOptions));

    ON_CALL (*instrumentation, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NilString));
    ON_CALL (*instrumentation, WrapperOptions ())
        .WillByDefault (ReturnRef (NilString));

    auto args (ycom::BuildCommandLine (ytest::ArgumentCount (originalArgs),
                                       ytest::Arguments (originalArgs),
                                       *instrumentation));

    Matcher <std::string> matchers[] = { StrEq (ytest::MockProgramName ()) };

    EXPECT_THAT (args,
                 ElementsAreArray (matchers));
}

TEST_F (BuildCommandLine, ReturnWrapperAndOptions)
{
    CommandLineArguments originalArgs (GenerateCommandLine (NoOptions));

    ON_CALL (*instrumentation, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockWrapper));
    ON_CALL (*instrumentation, WrapperOptions ())
        .WillByDefault (ReturnRef (MockOptions));

    auto args (ycom::BuildCommandLine (ytest::ArgumentCount (originalArgs),
                                       ytest::Arguments (originalArgs),
                                       *instrumentation));

    Matcher <std::string> matchers[] =
    {
        StrEq (MockWrapper),
        StrEq (ytest::MockProgramName ()),
        StrEq (MockOptions)
    };

    EXPECT_THAT (args,
                 ElementsAreArray (matchers));
}

TEST (CommandLine, VectorToArgv)
{
    std::string const MockItem ("item");
    std::string const MockItem2 ("item2");

    ycom::CommandArguments const args =
    {
        MockItem,
        MockItem2
    };

    Matcher <char const *> matchers[] =
    {
        StrEq (MockItem),
        StrEq (MockItem2),
    };

    ycom::ArgvVector const argv (ycom::StringVectorToArgv (args));

    EXPECT_THAT (argv,
                 ElementsAreArray (matchers));
}
