/*
 * reexecute_through_passthrough.cpp:
 *
 * Test running a sample binary through an
 * executable YIQI_PASSTHROUGH_EXECUTABLE passed by
 * the compiler. Verify by watching the stdout of that
 * executable
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include <iostream>
#include <system_error>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <fcntl.h>

#include <commandline.h>
#include <constants.h>

#include <passthrough_constants.h>

#include <acceptance_tests_config.h>

using ::testing::_;
using ::testing::AllOf;
using ::testing::Contains;
using ::testing::ElementsAreArray;
using ::testing::Matcher;
using ::testing::HasSubstr;

namespace bio = boost::iostreams;
namespace ycom = yiqi::commandline;
namespace yconst = yiqi::constants;
namespace yta = yiqi::testing::acceptance;
namespace ytp = yiqi::testing::passthrough;

namespace
{
    int launchBinary (std::string const         &executable,
                      ycom::NullTermArray const &argv,
                      ycom::NullTermArray const &env,
                      int                       &pipeWriteEnd)
    {
        /* Save stdout */
        int stdout = dup (STDOUT_FILENO);

        if (stdout == -1)
        {
            std::error_code code (errno,
                                  std::system_category ());
            throw std::system_error (code);
        }

        /* Drop reference */
        if (close (STDOUT_FILENO) == -1)
        {
            std::error_code code (errno,
                                  std::system_category ());
            throw std::system_error (code);
        }

        /* Make the pipe write end our stdout */
        if (dup2 (pipeWriteEnd, STDOUT_FILENO) == -1)
        {
            std::error_code code (errno,
                                  std::system_category ());
            throw std::system_error (code);
        }

        pid_t child = fork ();

        /* Child process */
        if (child == 0)
        {
            char * const *argvp (const_cast <char * const *> (
                                     argv.underlyingArray ()));
            char * const *envp (const_cast <char * const *> (
                                    env.underlyingArray ()));

            if (execvpe (executable.c_str (),
                         argvp,
                         envp) == -1)
            {
                std::cerr << "execvpe failed with error "
                          << errno
                          << std::endl
                          << " - binary "
                          << yta::passthrough
                          << std::endl;
                abort ();
            }
        }
        /* Parent process - error */
        else if (child == -1)
        {
            std::error_code code (errno,
                                  std::system_category ());
            throw std::system_error (code);
        }
        else
        {
            /* Redirect old stdout back to stdout */
            if (dup2 (stdout, STDOUT_FILENO) == -1)
            {
                std::error_code code (errno,
                                      std::system_category ());
                throw std::system_error (code);
            }

            /* Close the write end of the pipe - its being
             * used by the child */
            if (close (pipeWriteEnd) == -1)
            {
                std::error_code code (errno,
                                      std::system_category ());
                throw std::system_error (code);
            }

            pipeWriteEnd = 0;

            int status = 0;

            do
            {
                pid_t waitChild = waitpid (child, &status, 0);
                if (waitChild == child)
                {
                    if (WIFSIGNALED (status))
                    {
                        std::stringstream ss;
                        ss << "child killed by signal "
                           << WTERMSIG (status);
                        throw std::runtime_error (ss.str ());
                    }
                }
                else
                {
                    std::error_code code (errno,
                                          std::system_category ());
                    throw std::system_error (code);
                }
            } while (!WIFEXITED (status) && !WIFSIGNALED (status));

            return WEXITSTATUS (status);
        }

        throw std::logic_error ("unreachable section");
    }
}

class ChildOutputTest :
    public ::testing::Test
{
    public:

        ChildOutputTest () :
            env (environ)
        {
            if (pipe2 (childStdoutPipe, O_CLOEXEC) == -1)
            {
                std::error_code code (errno,
                                      std::system_category ());
                throw std::system_error (code);
            }
        }

        virtual void SetUp ()
        {
            argv.append (GetExecutable ());
        }

        virtual std::string GetExecutable () const = 0;

        ~ChildOutputTest ()
        {
            if (childStdoutPipe[0])
                close (childStdoutPipe[0]);

            if (childStdoutPipe[1])
                close (childStdoutPipe[1]);
        }

    protected:

        std::vector <std::string> GetChildOutput ();

        ycom::NullTermArray argv;
        ycom::NullTermArray env;
        std::string         lineBuffer;

        int                 childStdoutPipe[2];
};

class DirectlyExecutePassthrough :
    public ChildOutputTest
{
    public:

        std::string GetExecutable () const
        {
            return yta::passthrough;
        }
};

std::vector <std::string>
ChildOutputTest::GetChildOutput ()
{
    typedef bio::stream_buffer <bio::file_descriptor_source>  ChildStream;
    ChildStream streambuf (childStdoutPipe[0], bio::never_close_handle);

    std::vector <std::string> lines;

    std::istream stream (&streambuf);
    while (stream)
    {
        std::string line;
        std::getline (stream, line);
        if (!line.empty ())
            lines.push_back (line);
    }

    return lines;
}

TEST_F (DirectlyExecutePassthrough, ExecuteNoThrow)
{
    EXPECT_NO_THROW ({
        launchBinary (yta::passthrough,
                      argv,
                      env,
                      childStdoutPipe[1]);
    });
}

TEST_F (DirectlyExecutePassthrough, ExecuteNoArgsReturns1)
{
    EXPECT_EQ (1, launchBinary (yta::passthrough,
                                argv,
                                env,
                                childStdoutPipe[1]));
}

TEST_F (DirectlyExecutePassthrough, ExecuteWithNoExecArgsReturns1)
{
    std::string const MockArgument ("--ARGUMENT");
    argv.append (MockArgument);

    EXPECT_EQ (1, launchBinary (yta::passthrough,
                                argv,
                                env,
                                childStdoutPipe[1]));
}

TEST_F (DirectlyExecutePassthrough, ExecuteNoArgsOutputHeader)
{
    launchBinary (yta::passthrough,
                  argv,
                  env,
                  childStdoutPipe[1]);

    std::vector <std::string> output (GetChildOutput ());

    Matcher <std::string> expectedElements[] =
    {
        HasSubstr (ytp::StartupMessage)
    };

    EXPECT_THAT (output, ElementsAreArray (expectedElements));
}

TEST_F (DirectlyExecutePassthrough, ExecuteWithArgsOutputArg)
{
    std::string const MockArgument ("--ARGUMENT");
    argv.append (MockArgument);

    launchBinary (yta::passthrough,
                  argv,
                  env,
                  childStdoutPipe[1]);

    std::vector <std::string> output (GetChildOutput ());

    Matcher <std::string> expectedElements[] =
    {
        _,
        AllOf (HasSubstr (ytp::OptionHeader),
               HasSubstr (MockArgument))
    };

    EXPECT_THAT (output, ElementsAreArray (expectedElements));
}

namespace
{
    std::string const passthroughTool (
        yconst::StringFromTool (yconst::InstrumentationTool::Passthrough));
    std::string const dashdashYiqiToolOption (
        std::string ("--") + yconst::YiqiToolOption ());
}


class DirectlyExecuteSimpleTest :
    public ChildOutputTest
{
    public:

        DirectlyExecuteSimpleTest ()
        {
            env.removeAnyMatching ([](char const *str) -> bool {
                return strncmp (str, "PATH=", 5) == 0;
            });
            ycom::InsertEnvironmentPair (env,
                                         "PATH",
                                         yta::pthruPath.c_str ());
        }

        std::string GetExecutable () const
        {
            return yta::simpleTest;
        }
};

TEST_F (DirectlyExecuteSimpleTest, ExecuteNoThrow)
{
    EXPECT_NO_THROW ({
        launchBinary (yta::simpleTest,
                      argv,
                      env,
                      childStdoutPipe[1]);
    });
}

TEST_F (DirectlyExecuteSimpleTest, ExecuteWithToolArgsRunningUnderMsg)
{
    argv.append (dashdashYiqiToolOption);
    argv.append (passthroughTool);
    launchBinary (yta::simpleTest,
                  argv,
                  env,
                  childStdoutPipe[1]);

    std::vector <std::string> output (GetChildOutput ());

    EXPECT_THAT (output,
                 Contains (
                     AllOf (
                         HasSubstr (yconst::YiqiRunningUnderHeader ()),
                         HasSubstr (passthroughTool))));
}


TEST_F (DirectlyExecuteSimpleTest, ExecuteWithToolArgsOptionPassthroughMsg)
{
    argv.append (dashdashYiqiToolOption);
    argv.append (passthroughTool);
    launchBinary (yta::simpleTest,
                  argv,
                  env,
                  childStdoutPipe[1]);

    std::vector <std::string> output (GetChildOutput ());

    EXPECT_THAT (output,
                 Contains (HasSubstr (ytp::OptionHeader)));
}
