/*
 * reexecution.cpp:
 * Test that we can fetch the executable, argv and environment
 * from the system and pass it to a mocked out call to
 * ExecInPlace
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include "commandline.h"

#include "instrumentation_tool.h"
#include "system_api.h"

#include "instrumentation_mock.h"
#include "system_api_mock.h"

#include "array_fits_matchers.h"

#include <test_util.h>
#include <reexecution_tests_shared.h>

#include "constants.h"
#include "reexecution.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::IsNull;
using ::testing::MakeMatcher;
using ::testing::Matcher;
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

class ReExecutionIntegration :
    public ::testing::Test
{
    public:

        ReExecutionIntegration () :
            args (ytest::GenerateCommandLine (ytestrexec::InitialMockArgv))
        {
            tool.IgnoreCalls ();
            syscalls.IgnoreCalls ();

            /* Don't need the Tool Identifier */
            EXPECT_CALL (tool, ToolIdentifier ()).Times (0);

            /* Provide an environment */
            ON_CALL (syscalls, GetSystemEnvironment ())
                .WillByDefault (Return (ytestrexec::ProvidedEnvironment));
        }

    protected:

        ymockit::Tool               tool;
        ymocksysapi::SystemCalls    syscalls;
        ytest::CommandLineArguments args;
};


TEST_F (ReExecutionIntegration, SkipAndThrowIfNoInstrumentationWrapper)
{
    /* Never call the system calls */
    EXPECT_CALL (syscalls, ExeExists (_)).Times (0);
    EXPECT_CALL (syscalls, ExecInPlace (_, _, _)).Times (0);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (ytestrexec::NoInstrumentation));

    EXPECT_THROW ({
        yexec::RelaunchCurrentProgram (tool,
                                       ytest::ArgumentCount (args),
                                       ytest::Arguments (args),
                                       syscalls);
    }, std::logic_error);
}

TEST_F (ReExecutionIntegration, SkipAndThrowIfExecutableDoesNotExistInOnePath)
{
    std::string const ExpectedBinary (ytestrexec::MockExecutablePaths[0] +
                                      "/" +
                                      ytestrexec::MockInstrumentation);

    /* Never call ExecInPlace */
    EXPECT_CALL (syscalls, ExecInPlace (_, _, _)).Times (0);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (syscalls, GetExecutablePath ())
            .WillByDefault (Return (ytestrexec::MockExecutablePaths[0]));
    ON_CALL (syscalls, ExeExists (ExpectedBinary))
        .WillByDefault (Return (false));

    EXPECT_THROW ({
        yexec::RelaunchCurrentProgram (tool,
                                       ytest::ArgumentCount (args),
                                       ytest::Arguments (args),
                                       syscalls);
    }, std::runtime_error);
}

TEST_F (ReExecutionIntegration, SkipAndThrowIfExecutableDoesNotExistInTwoPaths)
{
    std::string const FirstCheckedBinary (ytestrexec::MockExecutablePaths[0] +
                                          "/" +
                                          ytestrexec::MockInstrumentation);
    std::string const SecondCheckedBinary (ytestrexec::MockExecutablePaths[0] +
                                           "/" +
                                           ytestrexec::MockInstrumentation);

    /* Never call ExecInPlace */
    EXPECT_CALL (syscalls, ExecInPlace (_, _, _)).Times (0);

    /* We should expect that ExeExists is called twice */
    EXPECT_CALL (syscalls, ExeExists (_)).Times (2);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (syscalls, GetExecutablePath ())
        .WillByDefault (Return (ytestrexec::MultiMockExecutablePath));
    ON_CALL (syscalls, ExeExists (FirstCheckedBinary))
        .WillByDefault (Return (false));
    ON_CALL (syscalls, ExeExists (SecondCheckedBinary))
        .WillByDefault (Return (false));

    EXPECT_THROW ({
        yexec::RelaunchCurrentProgram (tool,
                                       ytest::ArgumentCount (args),
                                       ytest::Arguments (args),
                                       syscalls);
    }, std::runtime_error);
}

TEST_F (ReExecutionIntegration, ExecIfExecutableExistsInSinglePathRightExe)
{
    std::string const ExpectedBinary (ytestrexec::MockExecutablePaths[0] +
                                      "/" +
                                      ytestrexec::MockInstrumentation);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));
    ON_CALL (syscalls, GetExecutablePath ())
            .WillByDefault (Return (ytestrexec::MockExecutablePaths[0]));
    ON_CALL (syscalls, ExeExists (ExpectedBinary))
        .WillByDefault (Return (true));

    EXPECT_CALL (syscalls, ExecInPlace (StrEq (ExpectedBinary),
                                         _,
                                         _))
        .Times (1);

    yexec::RelaunchCurrentProgram (tool,
                                   ytest::ArgumentCount (args),
                                   ytest::Arguments (args),
                                   syscalls);
}

TEST_F (ReExecutionIntegration, ExecIfExecutableExistsInSinglePathRightArgv)
{
    std::string const ExpectedBinary (ytestrexec::MockExecutablePaths[0] +
                                      "/" +
                                      ytestrexec::MockInstrumentation);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));
    ON_CALL (syscalls, GetExecutablePath ())
            .WillByDefault (Return (ytestrexec::MockExecutablePaths[0]));
    ON_CALL (syscalls, ExeExists (ExpectedBinary))
        .WillByDefault (Return (true));

    std::vector <Matcher <char const *> > matchers =
    {
        StrEq (ytestrexec::MockInstrumentation),
        StrEq (ytest::MockProgramName ()),
        StrEq (ytestrexec::MockArgument),
        IsNull ()
    };

    EXPECT_CALL (syscalls, ExecInPlace (_,
                                        ymatch::ArrayFitsMatchers (matchers),
                                        _))
        .Times (1);

    yexec::RelaunchCurrentProgram (tool,
                                   ytest::ArgumentCount (args),
                                   ytest::Arguments (args),
                                   syscalls);
}

TEST_F (ReExecutionIntegration, ExecIfExecutableExistsInSinglePathRightEnvp)
{
    std::string const ExpectedBinary (ytestrexec::MockExecutablePaths[0] +
                                      "/" +
                                      ytestrexec::MockInstrumentation);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));
    ON_CALL (syscalls, GetExecutablePath ())
            .WillByDefault (Return (ytestrexec::MockExecutablePaths[0]));
    ON_CALL (syscalls, ExeExists (ExpectedBinary))
        .WillByDefault (Return (true));

    ycom::NullTermArray ExpectedExecEnv (ytestrexec::ProvidedEnvironment);
    ycom::InsertEnvironmentPair (ExpectedExecEnv,
                                 yconst::YiqiToolEnvKey (),
                                 ytestrexec::MockInstrumentation.c_str ());

    auto ExpectedEnvp (ExpectedExecEnv.underlyingArray ());
    size_t ExpectedExecEnvLenSansNullTerminator =
        ExpectedExecEnv.underlyingArrayLen () - 1;

    std::vector <Matcher <char const *> > matchers;
    for (size_t i = 0;
         i < ExpectedExecEnvLenSansNullTerminator;
         ++i)
        matchers.push_back (StrEq (ExpectedEnvp[i]));

    EXPECT_CALL (syscalls, ExecInPlace (_,
                                        _,
                                        ymatch::ArrayFitsMatchers (matchers)))
        .Times (1);

    yexec::RelaunchCurrentProgram (tool,
                                   ytest::ArgumentCount (args),
                                   ytest::Arguments (args),
                                   syscalls);
}

TEST_F (ReExecutionIntegration, ExecIfExecutableExistsInSecondaryPath)
{
    std::string const FirstCheckedBinary (ytestrexec::MockExecutablePaths[0] +
                                           "/" +
                                          ytestrexec::MockInstrumentation);
    std::string const SecondCheckedBinary (ytestrexec::MockExecutablePaths[1] +
                                           "/" +
                                           ytestrexec::MockInstrumentation);

    /* Never call ExecInPlace */
    EXPECT_CALL (syscalls, ExecInPlace (_, _, _)).Times (0);

    /* We should expect that ExeExists is called twice */
    EXPECT_CALL (syscalls, ExeExists (_)).Times (2);

    ON_CALL (tool, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));
    ON_CALL (syscalls, GetExecutablePath ())
        .WillByDefault (Return (ytestrexec::MultiMockExecutablePath));
    ON_CALL (syscalls, ExeExists (FirstCheckedBinary))
        .WillByDefault (Return (false));
    ON_CALL (syscalls, ExeExists (SecondCheckedBinary))
        .WillByDefault (Return (true));

    char const * const                    *argv (ytest::Arguments (args));
    std::vector <Matcher <char const *> > matchers;
    for (int i = 0; i < ytest::ArgumentCount (args); ++i)
        matchers.push_back (StrEq (argv[i]));

    EXPECT_CALL (syscalls, ExecInPlace (StrEq (SecondCheckedBinary),
                                        _,
                                        _))
        .Times (1);

    yexec::RelaunchCurrentProgram (tool,
                                   ytest::ArgumentCount (args),
                                   ytest::Arguments (args),
                                   syscalls);
}
