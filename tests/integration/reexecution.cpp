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

            ON_CALL (tool, Name ())
                .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
        }

    protected:

        ymockit::Program            tool;
        ymocksysapi::SystemCalls    syscalls;
        ytest::CommandLineArguments args;
};


TEST_F (ReExecutionIntegration, SkipAndThrowIfNoWrapperBinary)
{
    /* Never call the system calls */
    EXPECT_CALL (syscalls, ExecInPlace (_, _, _)).Times (0);

    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::NoInstrumentation));

    EXPECT_THROW ({
        yexec::RelaunchCurrentProgram (tool,
                                       ytest::ArgumentCount (args),
                                       ytest::Arguments (args),
                                       syscalls);
    }, std::logic_error);
}

TEST_F (ReExecutionIntegration, ExecIfExecutableExistsInSinglePathRightExe)
{
    std::string const ExpectedBinary (ytestrexec::MockExecutablePaths[0] +
                                      "/" +
                                      ytestrexec::MockInstrumentation);

    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));
    ON_CALL (syscalls, LocateBinary (ytestrexec::MockInstrumentation))
        .WillByDefault (Return (ExpectedBinary));

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

    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));
    ON_CALL (syscalls, LocateBinary (ytestrexec::MockInstrumentation))
        .WillByDefault (Return (ExpectedBinary));

    std::vector <Matcher <char const *> > matchers =
    {
        StrEq (ytestrexec::MockInstrumentation),
        StrEq (ytestrexec::MockArgument),
        StrEq (ytest::MockProgramName),
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

    ON_CALL (tool, WrapperBinary ())
        .WillByDefault (ReturnRef (ytestrexec::MockInstrumentation));
    ON_CALL (tool, WrapperOptions ())
        .WillByDefault (ReturnRef (ytestrexec::MockArgument));
    ON_CALL (syscalls, LocateBinary (ytestrexec::MockInstrumentation))
        .WillByDefault (Return (ExpectedBinary));

    ycom::NullTermArray ExpectedExecEnv (ytestrexec::ProvidedEnvironment);
    ycom::InsertEnvironmentPair (ExpectedExecEnv,
                                 yconst::YiqiToolEnvKey,
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
