/*
 * reexecution.cpp:
 * Test that the rexecution logic calls through the mocked out
 * operating system as expected
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include "commandline.h"

#include "instrumentation_tool.h"
#include "system_api.h"

#include "instrumentation_mock.h"
#include "system_api_mock.h"

#include "test_util.h"

#include "reexecution.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Matcher;
using ::testing::MatcherInterface;
using ::testing::MatchResultListener;
using ::testing::MakeMatcher;
using ::testing::Message;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StrEq;

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

    std::string const MockExecutablePath ("/mock/executable/path/");
    std::string const ExecutableMockPath ("/executable/mock/path/");
    std::string const MultiMockExecutablePath (MockExecutablePath + ":" +
                                               ExecutableMockPath);

    std::vector <std::string> const InitialMockArgv =
    {
        ytest::MockProgramName (),
    };

    std::vector <std::string> const ExpectedMockArgv =
    {
        MockInstrumentation,
        ytest::MockProgramName (),
        MockArgument
    };
}

class ReExecution :
    public ::testing::Test
{
    public:

        ReExecution () :
            tool (new ymockit::Tool),
            syscalls (new ymocksysapi::SystemCalls),
            args (ytest::GenerateCommandLine (InitialMockArgv))
        {
            IgnoreCallsOnTool ();
            IgnoreCallsOnSyscalls ();
        }

        void IgnoreCallsOnTool ();
        void IgnoreCallsOnSyscalls ();

    protected:

        std::unique_ptr <ymockit::Tool>            tool;
        std::unique_ptr <ymocksysapi::SystemCalls> syscalls;
        ytest::CommandLineArguments                args;
};

void
ReExecution::IgnoreCallsOnSyscalls ()
{
    EXPECT_CALL (*syscalls, ExeExists (_)).Times (AtLeast (0));
    EXPECT_CALL (*syscalls, ExecInPlace (_, _)).Times (AtLeast (0));
    EXPECT_CALL (*syscalls, GetExecutablePath ()).Times (AtLeast (0));
}

void
ReExecution::IgnoreCallsOnTool ()
{
    EXPECT_CALL (*tool, InstrumentationWrapper ()).Times (AtLeast (0));
    EXPECT_CALL (*tool, WrapperOptions ()).Times (AtLeast (0));
}

TEST_F (ReExecution, SkipIfNoInstrumentationWrapper)
{
    /* Never call the system calls */
    EXPECT_CALL (*syscalls, ExeExists (_)).Times (0);
    EXPECT_CALL (*syscalls, ExecInPlace (_, _)).Times (0);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NoInstrumentation));

    yexec::RelaunchIfNecessary (ycom::ArgvVector (),
                                yexec::EnvironmentVector (),
                                *tool,
                                *syscalls);
}

TEST_F (ReExecution, NoExecIfExecutableDoesNotExistInSinglePath)
{
    std::string const ExpectedCheckedBinary (MockExecutablePath +
                                             "/" + MockInstrumentation);

    /* Never call ExecInPlace */
    EXPECT_CALL (*syscalls, ExecInPlace (_, _)).Times (0);

    ON_CALL (*tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockInstrumentation));
    ON_CALL (*syscalls, GetExecutablePath ())
        .WillByDefault (Return (MockExecutablePath));
    ON_CALL (*syscalls, ExeExists (ExpectedCheckedBinary))
        .WillByDefault (Return (false));

    yexec::RelaunchIfNecessary (ycom::ArgvVector (),
                                yexec::EnvironmentVector (),
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
    EXPECT_CALL (*syscalls, ExecInPlace (_, _)).Times (0);

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
                                yexec::EnvironmentVector (),
                                *tool,
                                *syscalls);
}

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
                                         ArrayFitsMatchers (matchers)))
        .Times (1);

    /* We need to convert to an ArgvVector here */
    ycom::ArgvVector vec (ytest::ToVector (argv, ytest::ArgumentCount (args)));

    yexec::RelaunchIfNecessary (vec,
                                yexec::EnvironmentVector (),
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
    EXPECT_CALL (*syscalls, ExecInPlace (_, _)).Times (0);

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
                                         ArrayFitsMatchers (matchers)))
        .Times (1);

    /* We need to convert to an ArgvVector here */
    ycom::ArgvVector vec (ytest::ToVector (argv, ytest::ArgumentCount (args)));

    yexec::RelaunchIfNecessary (vec,
                                yexec::EnvironmentVector (),
                                *tool,
                                *syscalls);
}
