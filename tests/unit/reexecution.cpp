/*
 * reexecution.cpp:
 * Test that the rexecution logic calls through the mocked out
 * operating system as expected
 *
 * See LICENCE.md for Copyright information
 */

#include <sstream>

#include <gmock/gmock.h>

#include "commandline.h"

#include "instrumentation_tool.h"
#include "system_api.h"

#include "instrumentation_mock.h"
#include "system_api_mock.h"

#include "array_fits_matchers.h"

#include "test_util.h"
#include "reexecution_tests_shared.h"

#include "constants.h"
#include "reexecution.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::IsNull;
using ::testing::Matcher;
using ::testing::MatcherInterface;
using ::testing::MatchResultListener;
using ::testing::MakeMatcher;
using ::testing::Message;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StrEq;

namespace yconst = yiqi::constants;
namespace ycom = yiqi::commandline;
namespace yexec = yiqi::execution;
namespace yit = yiqi::instrumentation::tools;
namespace ymatch = yiqi::matchers;
namespace ymock = yiqi::mock;
namespace ymockit = ymock::instrumentation::tools;
namespace ymocksysapi = ymock::system::api;
namespace ysysapi = yiqi::system::api;
namespace ytest = yiqi::testing;
namespace ytestrexec = yiqi::testing::reexecution;

class FindExecutable :
    public ::testing::Test
{
    public:

        FindExecutable ()
        {
            tool.IgnoreCalls ();
            syscalls.IgnoreCalls ();

            /* We don't need the tool ID */
            EXPECT_CALL (tool, ToolIdentifier ()).Times (0);

            /* We don't need the system environment */
            EXPECT_CALL (syscalls, GetSystemEnvironment ()).Times (0);
        }

    protected:

        ymockit::Program         tool;
        ymocksysapi::SystemCalls syscalls;
};

TEST_F (FindExecutable, ThrowLogicErrorOnEmptyWrapper)
{
    std::string const NoValue ("");
    ON_CALL (tool, WrapperBinary ()).WillByDefault (ReturnRef (NoValue));

    EXPECT_THROW ({
      yexec::FindExecutable (tool, syscalls);
    }, std::logic_error);
}

class GetArgvForTool :
    public ::testing::Test
{
    public:

        GetArgvForTool () :
            args (ytest::GenerateCommandLine (ytestrexec::InitialMockArgv))
        {
            tool.IgnoreCalls ();
        }

    protected:

        ymockit::Program            tool;
        ytest::CommandLineArguments args;
};

TEST_F (GetArgvForTool, AtLeastOneArgumentInReturnedArgvWithNoWrapper)
{
    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::NoInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    EXPECT_GE (argv.underlyingArrayLen (), 2UL); // 1 + null-term
}

TEST_F (GetArgvForTool, AtLeastTwoArgumentsInReturnedArgvWithWrapper)
{
    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    EXPECT_GE (argv.underlyingArrayLen (), 3UL); // 2 + null-term
}

TEST_F (GetArgvForTool, AtLeastThreeArgumentsInReturnedArgvWithWrapperAndOpts)
{
    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    EXPECT_GE (argv.underlyingArrayLen (), 4UL); // 3 + null-term
}

TEST_F (GetArgvForTool, FirstArgvIsInstrumentation)
{
    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    ASSERT_EQ (argv.underlyingArrayLen (), 3UL); // 2 + null-term
    EXPECT_THAT (argv.underlyingArray ()[0],
                 StrEq (ytestrexec::MockInstrumentation));
}

TEST_F (GetArgvForTool, SecondArgvIsProgramName)
{
    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    ASSERT_EQ (argv.underlyingArrayLen (), 3UL); // 2 + null-term
    EXPECT_THAT (argv.underlyingArray ()[1], StrEq (ytest::MockProgramName));
}

TEST_F (GetArgvForTool, SubsequentArgvAreOptions)
{
    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    std::vector <Matcher <char const *> > matchers =
    {
        _,
        StrEq (ytestrexec::MockArgument),
        _,
        IsNull ()
    };

    ASSERT_EQ (argv.underlyingArrayLen (), 4UL); // 3 + null-term
    EXPECT_THAT (argv.underlyingArray (),
                 ymatch::ArrayFitsMatchers (matchers));
}

TEST_F (GetArgvForTool, FinalArgvIsNull)
{
    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    ASSERT_EQ (argv.underlyingArrayLen (), 2UL); // 1 + null-term
    EXPECT_THAT (argv.underlyingArray ()[1], IsNull ());
}

class GetEnvForTool :
    public ::testing::Test
{
    public:

        GetEnvForTool ()
        {
            tool.IgnoreCalls ();
            syscalls.IgnoreCalls ();

            /* Don't need the tool id, wrapper or args */
            EXPECT_CALL (tool, ToolIdentifier ()).Times (0);
            EXPECT_CALL (tool, WrapperOptions ()).Times (0);
            EXPECT_CALL (tool, WrapperBinary ()).Times (0);

            /* Don't need to set the exec path
             * or exec things */
            EXPECT_CALL (syscalls, ExecInPlace (_, _, _)).Times (0);
            EXPECT_CALL (syscalls, GetExecutablePath ()).Times (0);

            /* By default, return the process environment */
            ON_CALL (syscalls, GetSystemEnvironment ())
                .WillByDefault (Return (ytestrexec::ProvidedEnvironment));
        }

    protected:

        ymockit::Program            tool;
        ymocksysapi::SystemCalls syscalls;
};

TEST_F (GetEnvForTool, ToolWithNoInstrumentationThrows)
{
    ON_CALL (tool, Name ())
        .WillByDefault (ReturnRef (ytestrexec::NoInstrumentation));
    EXPECT_THROW ({
        ycom::NullTermArray environment (yexec::GetToolEnv (tool,
                                                            syscalls));
    }, std::logic_error);
}

TEST_F (GetEnvForTool, MatchAtLeastTheFirstMembersInSysEnvironment)
{
    ON_CALL (tool, Name ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ycom::NullTermArray environment (yexec::GetToolEnv (tool,
                                                        syscalls));

    size_t const arrayLen = environment.underlyingArrayLen ();
    std::vector <Matcher <char const *> > matchers;
    char const * const *sysEnvPtr = ytestrexec::ProvidedEnvironment;

    for (size_t i = 0;
         i < arrayLen;
         ++i)
    {
        if (*sysEnvPtr)
            matchers.push_back (StrEq (*sysEnvPtr++));
        else
            matchers.push_back (_);
    }

    EXPECT_THAT (environment.underlyingArray (),
                 ymatch::ArrayFitsMatchers (matchers)); // tool env + null-term
}

TEST_F (GetEnvForTool, FirstMembersInSysEnvironmentThenInstrumentationEnv)
{
    ON_CALL (tool, Name ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ycom::NullTermArray environment (yexec::GetToolEnv (tool,
                                                        syscalls));

    size_t const arrayLen = environment.underlyingArrayLen ();
    std::vector <Matcher <char const *> > matchers;
    char const * const *sysEnvPtr = ytestrexec::ProvidedEnvironment;

    for (size_t i = 0;
         i < arrayLen;
         ++i)
    {
        if (*sysEnvPtr)
            matchers.push_back (StrEq (*sysEnvPtr++));
        else if (i < (arrayLen - 1))
        {
            std::stringstream ss;
            ss << yconst::YiqiToolEnvKey
               << "="
               << ytestrexec::MockInstrumentation;
            matchers.push_back (StrEq (ss.str ()));
        }
        else
            matchers.push_back (IsNull ());
    }

    EXPECT_THAT (environment.underlyingArray (),
                 ymatch::ArrayFitsMatchers (matchers)); // tool env + null-term
}

namespace
{
    class MockFetchFunctions
    {
        public:

            MOCK_METHOD2 (Executable,
                          std::string (yit::Program const         &,
                                       ysysapi::SystemCalls const &));
            MOCK_METHOD1 (Argv,
                          ycom::NullTermArray (yit::Program const &));
            MOCK_METHOD2 (Environment,
                          ycom::NullTermArray (yit::Program         const &,
                                               ysysapi::SystemCalls const &));
    };
}

class Relaunch :
    public ::testing::Test
{
    public:

        Relaunch ()
        {
            syscalls.IgnoreCalls ();

            /* We only care about ExecInPlace */
            EXPECT_CALL (syscalls, GetExecutablePath ()).Times (0);
            EXPECT_CALL (syscalls, GetSystemEnvironment ()).Times (0);

            /* No calls to tool */
            EXPECT_CALL (tool, WrapperBinary ()).Times (0);
            EXPECT_CALL (tool, WrapperOptions ()).Times (0);
            EXPECT_CALL (tool, ToolIdentifier ()).Times (0);
        }

    protected:

        MockFetchFunctions       fetch;
        ymocksysapi::SystemCalls syscalls;
        ymockit::Program         tool;
};

TEST_F (Relaunch, VerifyCStyleExecutable)
{
    using namespace std::placeholders;

    std::string const MockExecutableLocation ("/mock/executable/location");

    EXPECT_CALL (fetch, Executable (_, _))
        .WillOnce (Return (MockExecutableLocation));

    EXPECT_CALL (syscalls, ExecInPlace (StrEq (MockExecutableLocation),
                                        _,
                                        _));

    yexec::Relaunch (tool,
                     std::bind (&MockFetchFunctions::Executable, &fetch,
                                _1, _2),
                     [](yit::Program const &t) {
                         return yexec::NullTermArray ();
                     },
                     [](yit::Program const &t, ysysapi::SystemCalls const &c) {
                         return yexec::NullTermArray ();
                     },
                     syscalls);
}

TEST_F (Relaunch, VerifyCStyleArguments)
{
    using namespace std::placeholders;

    std::vector <char const *> const ArgvVector =
    {
        "Item1",
        "Item2",
        "Item3",
        nullptr
    };
    ycom::NullTermArray const ArgvArray (&ArgvVector[0]);

    std::vector <Matcher <char const *> > matchers;
    for (char const *argvItem : ArgvVector)
    {
        if (argvItem)
            matchers.push_back (StrEq (argvItem));
        else
            matchers.push_back (IsNull ());
    }

    EXPECT_CALL (fetch, Argv (_))
        .WillOnce (Return (ArgvArray));

    EXPECT_CALL (syscalls, ExecInPlace (_,
                                        ymatch::ArrayFitsMatchers (matchers),
                                        _));

    yexec::Relaunch (tool,
                     [](yit::Program const &t, ysysapi::SystemCalls const &c) {
                         return std::string ();
                     },
                     std::bind (&MockFetchFunctions::Argv, &fetch, _1),
                     [](yit::Program const &t, ysysapi::SystemCalls const &c) {
                         return yexec::NullTermArray ();
                     },
                     syscalls);
}

TEST_F (Relaunch, VerifyCStyleEnv)
{
    using namespace std::placeholders;

    std::vector <char const *> const EnvVector =
    {
        "ITEM1=Item1",
        "ITEM2=Item2",
        "ITEM3=Item3",
        nullptr
    };
    ycom::NullTermArray const EnvArray (&EnvVector[0]);

    std::vector <Matcher <char const *> > matchers;
    for (char const *envItem : EnvVector)
    {
        if (envItem)
            matchers.push_back (StrEq (envItem));
        else
            matchers.push_back (IsNull ());
    }

    EXPECT_CALL (fetch, Environment (_, _))
        .WillOnce (Return (EnvArray));

    EXPECT_CALL (syscalls, ExecInPlace (_,
                                        _,
                                        ymatch::ArrayFitsMatchers (matchers)));

    yexec::Relaunch (tool,
                     [](yit::Program const &t, ysysapi::SystemCalls const &c) {
                         return std::string ();
                     },
                     [](yit::Program const &t) {
                         return yexec::NullTermArray ();
                     },
                     std::bind (&MockFetchFunctions::Environment, &fetch,
                                _1, _2),
                     syscalls);
}
