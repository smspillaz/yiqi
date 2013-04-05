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

#include "test_util.h"

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
namespace ymock = yiqi::mock;
namespace ymockit = ymock::instrumentation::tools;
namespace ymocksysapi = ymock::system::api;
namespace ysysapi = yiqi::system::api;
namespace ytest = yiqi::testing;

namespace
{
    std::string const NoInstrumentation ("");
    std::string const MockInstrumentation ("mocktool");
    std::string const MockArgument ("--mock");

    std::string const NoExecutablePath ("");
    std::vector <std::string> const MockExecutablePaths =
    {
        std::string ("/mock/executable/path/"),
        std::string ("/executable/mock/path/")
    };
    std::string const MultiMockExecutablePath (MockExecutablePaths[0] + ":" +
                                               MockExecutablePaths[1]);

    std::vector <std::string> const InitialMockArgv =
    {
    };

    std::vector <std::string> const ExpectedMockArgv =
    {
        MockInstrumentation,
        ytest::MockProgramName (),
        MockArgument
    };

    std::string const MockEnv ("MOCK=mock");
    char const * const ProvidedEnvironment[] =
    {
        MockEnv.c_str (),
        NULL
    };
}

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

        ymockit::Tool            tool;
        ymocksysapi::SystemCalls syscalls;
};

TEST_F (FindExecutable, ThrowLogicErrorOnEmptyWrapper)
{
    std::string const NoValue ("");
    ON_CALL (tool, InstrumentationWrapper ()).WillByDefault (ReturnRef (NoValue));

    EXPECT_THROW ({
      yexec::FindExecutable (tool, syscalls);
    }, std::logic_error);
}

TEST_F (FindExecutable, ThrowRuntimeErrorOnEmptyPath)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (syscalls, GetExecutablePath ())
        .WillByDefault (Return (NoExecutablePath));

    EXPECT_THROW ({
      yexec::FindExecutable (tool, syscalls);
    }, std::runtime_error);
}

TEST_F (FindExecutable, ThrowRuntimeErrorOnNotFoundInOnePath)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (syscalls, GetExecutablePath ())
            .WillByDefault (Return (MockExecutablePaths[0]));
    ON_CALL (syscalls, ExeExists (_))
        .WillByDefault (Return (false));

    EXPECT_THROW ({
      yexec::FindExecutable (tool, syscalls);
    }, std::runtime_error);
}

TEST_F (FindExecutable, ThrowRuntimeErrorOnNotFoundInMultiplePaths)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (syscalls, GetExecutablePath ())
            .WillByDefault (Return (MultiMockExecutablePath));
    ON_CALL (syscalls, ExeExists (_))
        .WillByDefault (Return (false));

    EXPECT_THROW ({
      yexec::FindExecutable (tool, syscalls);
    }, std::runtime_error);
}

TEST_F (FindExecutable, ReturnFullyQualifiedPathIfFoundInOnePath)
{
    std::string const ExpectedPath (MockExecutablePaths[0] +
                                    "/" +
                                    MockInstrumentation);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (syscalls, GetExecutablePath ())
            .WillByDefault (Return (MockExecutablePaths[0]));
    ON_CALL (syscalls, ExeExists (ExpectedPath))
        .WillByDefault (Return (true));

    EXPECT_EQ (ExpectedPath, yexec::FindExecutable (tool, syscalls));
}

TEST_F (FindExecutable, ReturnFullyQualifiedPathIfFoundOtherPath)
{
    std::string const NoFindPath (MockExecutablePaths[0] +
                                  "/" +
                                  MockInstrumentation);
    std::string const ExpectedPath (MockExecutablePaths[1] +
                                    "/" +
                                    MockInstrumentation);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (syscalls, GetExecutablePath ())
        .WillByDefault (Return (MultiMockExecutablePath));
    ON_CALL (syscalls, ExeExists (NoFindPath))
        .WillByDefault (Return (false));
    ON_CALL (syscalls, ExeExists (ExpectedPath))
        .WillByDefault (Return (true));

    /* We really should be checking twice */
    EXPECT_CALL (syscalls, ExeExists (_)).Times (2);

    EXPECT_EQ (ExpectedPath, yexec::FindExecutable (tool, syscalls));
}

#if 0

class ReExecution :
    public ::testing::Test
{
    public:

        ReExecution () :
            args (ytest::GenerateCommandLine (InitialMockArgv)),
            env (ProvidedEnvironment)
        {
            syscalls.IgnoreCalls ();

            /* We don't need the system environment, executable path
             * or to determine if any executables exist. All that has been
             * done for us already */
            EXPECT_CALL (syscalls, GetSystemEnvironment ()).Times (0);
            EXPECT_CALL (syscalls, GetExecutablePath ()).Times (0);
            EXPECT_CALL (syscalls, ExeExists (_)).Times (0);
        }

    protected:

        ymocksysapi::SystemCalls    syscalls;
        ytest::CommandLineArguments args;
        ycom::ArgvVector            argvVector;
        ycom::NullTermArray         argv;
        ycom::NullTermArray         env;
};

TEST_F (ReExecution, SkipIfNoInstrumentationWrapper)
{
    /* Never call the system calls */
    EXPECT_CALL (*syscalls, ExeExists (_)).Times (0);
    EXPECT_CALL (*syscalls, ExecInPlace (_, _, _)).Times (0);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NoInstrumentation));

    yexec::RelaunchIfNecessary (ycom::ArgvVector (),
                                ycom::NullTermArray (),
                                *tool,
                                *syscalls);
}

TEST_F (ReExecution, NoExecIfExecutableDoesNotExistInSinglePath)
{
    std::string const ExpectedCheckedBinary (MockExecutablePath +
                                             "/" + MockInstrumentation);

    /* Never call ExecInPlace */
    EXPECT_CALL (*syscalls, ExecInPlace (_, _, _)).Times (0);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (*syscalls, GetExecutablePath ())
        .WillByDefault (Return (MockExecutablePath));
    ON_CALL (*syscalls, ExeExists (ExpectedCheckedBinary))
        .WillByDefault (Return (false));

    yexec::RelaunchIfNecessary (ycom::ArgvVector (),
                                ycom::NullTermArray (),
                                *tool,
                                *syscalls);
}

TEST_F (ReExecution, NoExecIfExecutableDoesNotExistInMultiplePaths)
{
    std::string const FirstExpectedCheckedBinary (MockExecutablePath +
                                                  "/" + MockInstrumentation);
    std::string const SecondExpectedCheckedBinary (MockExecutablePath +
                                                   "/" + MockInstrumentation);

    /* Never call ExecInPlace */
    EXPECT_CALL (*syscalls, ExecInPlace (_, _, _)).Times (0);

    /* We should expect that ExeExists is called twice */
    EXPECT_CALL (*syscalls, ExeExists (_)).Times (2);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (*syscalls, GetExecutablePath ())
        .WillByDefault (Return (MultiMockExecutablePath));
    ON_CALL (*syscalls, ExeExists (FirstExpectedCheckedBinary))
        .WillByDefault (Return (false));
    ON_CALL (*syscalls, ExeExists (SecondExpectedCheckedBinary))
        .WillByDefault (Return (false));

    yexec::RelaunchIfNecessary (ycom::ArgvVector (),
                                ycom::NullTermArray (),
                                *tool,
                                *syscalls);
}

#endif

template <typename ArrayType>
class ArrayMatcher :
    public MatcherInterface <ArrayType const *>
{
    public:

        ArrayMatcher (std::vector <Matcher <ArrayType> > const &matchers) :
            mMatchers (matchers)
        {
        }

        void DescribeTo (std::ostream *os) const
        {
            size_t n (count ());

            if (n == 0)
            {
                *os << "matching no elements";
                return;
            }
            else if (n == 1)
            {
                *os << "matches the first element which is ";
                mMatchers[0].DescribeTo (os);
            }
            else
            {
                *os << "matches the first "
                    << n << " elements which are"
                    << std::endl;

                for (size_t i = 0; i < n; ++i)
                {
                    *os << "element #" << n << " ";
                    mMatchers[i].DescribeTo (os);
                    *os << "," << std::endl;
                }
            }
        }

        void DescribeNegationTo (std::ostream *os) const
        {
            size_t n (count ());

            ASSERT_TRUE (n > 0) << "matching zero elements must "
                                   "always pass";

            if (n == 1)
            {
                *os << "doesn't match the first element which is ";
                mMatchers[0].DescribeTo (os);
            }
            else
            {
                *os << "doesn't match the first "
                    << n << " elements which are"
                    << std::endl;

                for (size_t i = 0; i < n; ++i)
                {
                    *os << "element #" << n << " ";
                    mMatchers[i].DescribeTo (os);
                    *os << "," << std::endl;
                }
            }
        }

        bool MatchAndExplain (ArrayType const     *arr,
                              MatchResultListener *listener) const
        {
            size_t n (count ());

            /* We can pass instantly if we are not matching any
             * of the elements */
            if (n == 0)
                return true;
            else
            {
                if (arr)
                {
                    for (size_t i = 0; i < n; ++i)
                    {
                        ::testing::internal::StringMatchResultListener s;
                        bool success = mMatchers[i].MatchAndExplain (arr[i],
                                                                     &s);

                        if (!success)
                        {
                            std::ostream *stream (listener->stream ());
                            *listener << "whose element #" << i
                                      << "doesn't match";
                            ::testing::internal::PrintIfNotEmpty (s.str (),
                                                                  stream);
                            return false;
                        }
                    }
                }
                else
                {
                    std::ostream *os = listener->stream ();
                    if (os)
                        *os << std::string ("tries to match elements in an "
                                            "empty array") << std::endl;

                    return false;
                }
            }

            return true;
        }

    private:

        size_t count () const
        {
            return mMatchers.size ();
        }

        std::vector <Matcher <ArrayType> > const mMatchers;
};

template <typename T>
inline Matcher <T const *> ArrayFitsMatchers (std::vector <Matcher <T> > const &matchers)
{
    return MakeMatcher (new ArrayMatcher <T> (matchers));
}

class GetArgvForTool :
    public ::testing::Test
{
    public:

        GetArgvForTool () :
            args (ytest::GenerateCommandLine (InitialMockArgv))
        {
            tool.IgnoreCalls ();
        }

    protected:

        ymockit::Tool               tool;
        ytest::CommandLineArguments args;
};

TEST_F (GetArgvForTool, AtLeastOneArgumentInReturnedArgvWithNoWrapper)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NoInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    EXPECT_GE (argv.underlyingArrayLen (), 2); // 1 + null-term
}

TEST_F (GetArgvForTool, AtLeastTwoArgumentsInReturnedArgvWithWrapper)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    EXPECT_GE (argv.underlyingArrayLen (), 3); // 2 + null-term
}

TEST_F (GetArgvForTool, AtLeastThreeArgumentsInReturnedArgvWithWrapperAndOpts)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (MockArgument));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    EXPECT_GE (argv.underlyingArrayLen (), 4); // 3 + null-term
}

TEST_F (GetArgvForTool, FirstArgvIsInstrumentation)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    ASSERT_EQ (argv.underlyingArrayLen (), 3); // 2 + null-term
    EXPECT_THAT (argv.underlyingArray ()[0], StrEq (MockInstrumentation));
}

TEST_F (GetArgvForTool, SecondArgvIsProgramName)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    ASSERT_EQ (argv.underlyingArrayLen (), 3); // 2 + null-term
    EXPECT_THAT (argv.underlyingArray ()[1], StrEq (ytest::MockProgramName ()));
}

TEST_F (GetArgvForTool, SubsequentArgvAreOptions)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (MockArgument));

    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    std::vector <Matcher <char const *> > matchers =
    {
        _,
        _,
        StrEq (MockArgument),
        IsNull ()
    };

    ASSERT_EQ (argv.underlyingArrayLen (), 4); // 3 + null-term
    EXPECT_THAT (argv.underlyingArray (),
                 ArrayFitsMatchers (matchers));
}

TEST_F (GetArgvForTool, FinalArgvIsNull)
{
    ycom::NullTermArray argv (yexec::GetToolArgv (tool,
                                                  ytest::ArgumentCount (args),
                                                  ytest::Arguments (args)));

    ASSERT_EQ (argv.underlyingArrayLen (), 2); // 1 + null-term
    EXPECT_THAT (argv.underlyingArray ()[1], IsNull ());
}

class GetEnvForTool :
    public ::testing::Test
{
    public:

        GetEnvForTool ()
        {
            tool.IgnoreCalls();
            syscalls.IgnoreCalls ();

            /* Don't need the tool id or args */
            EXPECT_CALL (tool, ToolIdentifier ()).Times (0);
            EXPECT_CALL (tool, WrapperOptions ()).Times (0);

            /* Don't need to check if executables exist, the exec path
             * or exec things */
            EXPECT_CALL (syscalls, ExeExists (_)).Times (0);
            EXPECT_CALL (syscalls, ExecInPlace (_, _, _)).Times (0);
            EXPECT_CALL (syscalls, GetExecutablePath ()).Times (0);

            /* By default, return the process environment */
            ON_CALL (syscalls, GetSystemEnvironment ())
                .WillByDefault (Return (ProvidedEnvironment));
        }

    protected:

        ymockit::Tool            tool;
        ymocksysapi::SystemCalls syscalls;
};

TEST_F (GetEnvForTool, AtLeastTwoMembers)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NoInstrumentation));
    ycom::NullTermArray environment (yexec::GetToolEnv (tool,
                                                        syscalls));

    EXPECT_GE (environment.underlyingArrayLen (), 2); // tool env + null-term
}

TEST_F (GetEnvForTool, MatchAtLeastTheFirstMembersInSysEnvironment)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NoInstrumentation));
    ycom::NullTermArray environment (yexec::GetToolEnv (tool,
                                                        syscalls));

    size_t const arrayLen = environment.underlyingArrayLen ();
    std::vector <Matcher <char const *> > matchers;
    char const * const *sysEnvPtr = ProvidedEnvironment;

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
                 ArrayFitsMatchers (matchers)); // tool env + null-term
}

TEST_F (GetEnvForTool, OnlyTheFirstMembersInSysEnvironmentIfNoInstrumentation)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NoInstrumentation));
    ycom::NullTermArray environment (yexec::GetToolEnv (tool,
                                                        syscalls));

    size_t const arrayLen = environment.underlyingArrayLen ();
    std::vector <Matcher <char const *> > matchers;
    char const * const *sysEnvPtr = ProvidedEnvironment;

    for (size_t i = 0;
         i < arrayLen;
         ++i)
    {
        if (*sysEnvPtr)
            matchers.push_back (StrEq (*sysEnvPtr++));
        else
            matchers.push_back (IsNull ());
    }

    EXPECT_THAT (environment.underlyingArray (),
                 ArrayFitsMatchers (matchers)); // tool env + null-term
}

TEST_F (GetEnvForTool, FirstMembersInSysEnvironmentThenInstrumentationEnv)
{
    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ycom::NullTermArray environment (yexec::GetToolEnv (tool,
                                                        syscalls));

    size_t const arrayLen = environment.underlyingArrayLen ();
    std::vector <Matcher <char const *> > matchers;
    char const * const *sysEnvPtr = ProvidedEnvironment;

    for (size_t i = 0;
         i < arrayLen;
         ++i)
    {
        if (*sysEnvPtr)
            matchers.push_back (StrEq (*sysEnvPtr++));
        else if (i < (arrayLen - 1))
        {
            std::stringstream ss;
            ss << yconst::YiqiToolEnvKey ()
               << "="
               << MockInstrumentation;
            matchers.push_back (StrEq (ss.str ()));
        }
        else
            matchers.push_back (IsNull ());
    }

    EXPECT_THAT (environment.underlyingArray (),
                 ArrayFitsMatchers (matchers)); // tool env + null-term
}

namespace
{
    class MockFetchFunctions
    {
        public:

            MOCK_CONST_METHOD2 (Executable,
                                std::string (yit::Tool const            &,
                                             ysysapi::SystemCalls const &));
            MOCK_CONST_METHOD1 (Argv,
                                ycom::NullTermArray (yit::Tool const &));
            MOCK_CONST_METHOD2 (Environment,
                                ycom::NullTermArray (yit::Tool            const &,
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
            EXPECT_CALL (syscalls, ExeExists (_)).Times (0);
            EXPECT_CALL (syscalls, GetExecutablePath ()).Times (0);
            EXPECT_CALL (syscalls, GetSystemEnvironment ()).Times (0);

            /* No calls to tool */
            EXPECT_CALL (tool, InstrumentationWrapper ()).Times (0);
            EXPECT_CALL (tool, WrapperOptions ()).Times (0);
            EXPECT_CALL (tool, ToolIdentifier ()).Times (0);
        }

    protected:

        MockFetchFunctions       fetch;
        ymocksysapi::SystemCalls syscalls;
        ymockit::Tool            tool;
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
                     [](yit::Tool const &t) {
                         return yexec::NullTermArray ();
                     },
                     [](yit::Tool const &t, ysysapi::SystemCalls const &c) {
                         return yexec::NullTermArray ();
                     },
                     syscalls);
}

namespace
{

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
                                        ArrayFitsMatchers (matchers),
                                        _));

    yexec::Relaunch (tool,
                     [](yit::Tool const &t, ysysapi::SystemCalls const &c) {
                         return std::string ();
                     },
                     std::bind (&MockFetchFunctions::Argv, &fetch, _1),
                     [](yit::Tool const &t, ysysapi::SystemCalls const &c) {
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
                                        ArrayFitsMatchers (matchers)));

    yexec::Relaunch (tool,
                     [](yit::Tool const &t, ysysapi::SystemCalls const &c) {
                         return std::string ();
                     },
                     [](yit::Tool const &t) {
                         return yexec::NullTermArray ();
                     },
                     std::bind (&MockFetchFunctions::Environment, &fetch,
                                _1, _2),
                     syscalls);
}

#if 0

/*
 * These functions are doing too much:
 * 1. Fetching the instrumentation tool name
 * 2. Finding the right binary
 * 3. Setting the env
 * 4. Creating a null-terminated argv list
 * 5. calling ExecInPlace
 */
TEST_F (ReExecution, ExecIfExecutableExistsInSinglePath)
{
    std::string const ExpectedCheckedBinary (MockExecutablePath +
                                             "/" + MockInstrumentation);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (*tool, WrapperOptions ())
        .WillByDefault (ReturnRef (MockArgument));
    ON_CALL (*syscalls, GetExecutablePath ())
        .WillByDefault (Return (MockExecutablePath));
    ON_CALL (*syscalls, ExeExists (ExpectedCheckedBinary))
        .WillByDefault (Return (true));

    char const * const                    *argv (ytest::Arguments (args));
    std::vector <Matcher <char const *> > matchers;
    for (int i = 0; i < ytest::ArgumentCount (args); ++i)
        matchers.push_back (StrEq (argv[i]));

    EXPECT_CALL (*syscalls, ExecInPlace (StrEq (ExpectedCheckedBinary),
                                         ArrayFitsMatchers (matchers),
                                         _))
        .Times (1);

    /* We need to convert to an ArgvVector here */
    ycom::ArgvVector vec (ytest::ToVector (argv, ytest::ArgumentCount (args)));

    yexec::RelaunchIfNecessary (vec,
                                ycom::NullTermArray (),
                                *tool,
                                *syscalls);
}

TEST_F (ReExecution, ExecIfExecutableExistsInSinglePathWithAppendedEnv)
{
    std::string const ExpectedCheckedBinary (MockExecutablePath +
                                             "/" + MockInstrumentation);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (*tool, WrapperOptions ())
        .WillByDefault (ReturnRef (MockArgument));
    ON_CALL (*syscalls, GetExecutablePath ())
        .WillByDefault (Return (MockExecutablePath));
    ON_CALL (*syscalls, ExeExists (ExpectedCheckedBinary))
        .WillByDefault (Return (true));

    ycom::NullTermArray ExpectedExecEnv (env);
    ExpectedExecEnv.insert (yconst::YiqiToolEnvKey (),
                            MockInstrumentation.c_str ());

    auto ExpectedEnvp (ExpectedExecEnv.underlyingEnvironmentArray ());
    size_t ExpectedExecEnvLenSansNullTerminator =
        ExpectedExecEnv.underlyingEnvironmentArrayLen () - 1;

    std::vector <Matcher <char const *> > matchers;
    for (size_t i = 0;
         i < ExpectedExecEnvLenSansNullTerminator;
         ++i)
        matchers.push_back (StrEq (ExpectedEnvp[i]));

    EXPECT_CALL (*syscalls, ExecInPlace (StrEq (ExpectedCheckedBinary),
                                         _,
                                         ArrayFitsMatchers (matchers)))
        .Times (1);
    yexec::RelaunchIfNecessary (ycom::ArgvVector (),
                                env,
                                *tool,
                                *syscalls);
}

TEST_F (ReExecution, ExecIfExecutableExistsInSecondaryPath)
{
    std::string const FirstExpectedCheckedBinary (MockExecutablePath +
                                                  "/" + MockInstrumentation);
    std::string const SecondExpectedCheckedBinary (ExecutableMockPath +
                                                   "/" + MockInstrumentation);

    /* Never call ExecInPlace */
    EXPECT_CALL (*syscalls, ExecInPlace (_, _, _)).Times (0);

    /* We should expect that ExeExists is called twice */
    EXPECT_CALL (*syscalls, ExeExists (_)).Times (2);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (*tool, WrapperOptions ())
        .WillByDefault (ReturnRef (MockArgument));
    ON_CALL (*syscalls, GetExecutablePath ())
        .WillByDefault (Return (MultiMockExecutablePath));
    ON_CALL (*syscalls, ExeExists (FirstExpectedCheckedBinary))
        .WillByDefault (Return (false));
    ON_CALL (*syscalls, ExeExists (SecondExpectedCheckedBinary))
        .WillByDefault (Return (true));

    char const * const                    *argv (ytest::Arguments (args));
    std::vector <Matcher <char const *> > matchers;
    for (int i = 0; i < ytest::ArgumentCount (args); ++i)
        matchers.push_back (StrEq (argv[i]));

    EXPECT_CALL (*syscalls, ExecInPlace (StrEq (SecondExpectedCheckedBinary),
                                         ArrayFitsMatchers (matchers),
                                         _))
        .Times (1);

    /* We need to convert to an ArgvVector here */
    ycom::ArgvVector vec (ytest::ToVector (argv, ytest::ArgumentCount (args)));

    yexec::RelaunchIfNecessary (vec,
                                ycom::NullTermArray (),
                                *tool,
                                *syscalls);
}

#endif
