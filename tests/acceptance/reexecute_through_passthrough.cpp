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

#include <boost/noncopyable.hpp>

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
    class Pipe :
        boost::noncopyable
    {
        public:

            Pipe ()
            {
                if (pipe2 (mPipe, O_CLOEXEC) == -1)
                    throw std::runtime_error (strerror (errno));
            }

            ~Pipe ()
            {
                if (mPipe[0] &&
                    close (mPipe[0]) == -1)
                    std::cerr << "mPipe[0] " << strerror (errno) << std::endl;

                if (mPipe[1] &&
                    close (mPipe[1]) == -1)
                    std::cerr << "mPipe[0] " << strerror (errno) << std::endl;
            }

            /* Read end descriptor is read-only */
            int ReadEnd ()
            {
                return mPipe[0];
            }

            /* Write end descriptor is writable, we need to close it
             * from other objects */
            int & WriteEnd ()
            {
                return mPipe[1];
            }

        private:

            int mPipe[2];
    };

    class FileDescriptorBackup :
        boost::noncopyable
    {
        public:

            FileDescriptorBackup (int fd) :
                mOriginalFd (fd),
                mBackupFd (0)
            {
                mBackupFd = dup (mOriginalFd);

                /* Save original */
                if (mBackupFd == -1)
                    throw std::runtime_error (strerror (errno));
            }

            ~FileDescriptorBackup ()
            {
                /* Redirect backed up fd to old fd location */
                if (mBackupFd &&
                    dup2 (mBackupFd, mOriginalFd) == -1)
                    std::cerr << "Failed to restore file descriptor "
                              << strerror (errno) << std::endl;
            }

        private:

            int mOriginalFd;
            int mBackupFd;
    };

    class RedirectedFileDescriptor :
        boost::noncopyable
    {
        public:

            RedirectedFileDescriptor (int from,
                                      int &to) :
                mFromFd (from),
                mToFd (to)
            {
                /* Make 'to' take the old file descriptor's place */
                if (dup2 (to, from) == -1)
                    throw std::runtime_error (strerror (errno));
            }

            ~RedirectedFileDescriptor ()
            {
                if (mToFd &&
                    close (mToFd) == -1)
                    std::cerr << "Failed to close redirect-to file descriptor "
                    << strerror (errno) << std::endl;

                mToFd = 0;
            }

        private:

        int mFromFd;
        int &mToFd;
    };

    pid_t launchBinary (std::string const  &executable,
                        char const * const *argv,
                        char const * const *env,
                        int                &stdoutWriteEnd)
    {
        FileDescriptorBackup stdout (STDOUT_FILENO);

        /* Close the originals once they have been backed up
         * We have to do this here and not in the FileDescriptorBackup
         * constructors because of an order-of-operations issue -
         * namely if we close an original file descriptor name
         * before duplicating another one, then there's a possibility
         * that the duplicated other one will get the same name as
         * the one we just closed, making us unable to restore
         * the closed one properly */

        if (close (STDOUT_FILENO) == -1)
            throw std::runtime_error (strerror (errno));

        /* Replace the current process stderr and stdout with the write end
         * of the pipes. Now when someone tries to write to stderr or stdout
         * they'll write to our pipes instead */
        RedirectedFileDescriptor pipedStdout (STDOUT_FILENO, stdoutWriteEnd);

        /* Fork process, child gets a copy of the pipe write ends
         * - the original pipe write ends will be closed on exec
         * but the duplicated write ends now taking the place of
         * stderr and stdout will not be */
        pid_t child = fork ();

        /* Child process */
        if (child == 0)
        {
            if (execvpe (executable.c_str (),
                const_cast <char * const *> (argv),
                const_cast <char * const *> (env)) == -1)
            {
                std::cerr << "execvpe failed with error "
                          << errno
                          << "\n"
                          << " - binary "
                          << executable
                          << std::endl;
                abort ();
            }
        }
        /* Parent process - error */
        else if (child == -1)
            throw std::runtime_error (strerror (errno));

        /* The old file descriptors for the stderr and stdout
         * are put back in place, and pipe write ends closed
         * as the child is using them at return */

        return child;
    }

    int launchBinaryAndWaitForReturn (std::string const         &executable,
                                      ycom::NullTermArray const &argv,
                                      ycom::NullTermArray const &env,
                                      int                       &stdoutWrite)
    {
        int status = 0;
        pid_t child = launchBinary (executable,
                                    argv.underlyingArray (),
                                    env.underlyingArray (),
                                    stdoutWrite);

        do
        {
            /* Wait around for the child to get a signal */
            pid_t waitChild = waitpid (child, &status, 0);
            if (waitChild == child)
            {
                /* If it died unexpectedly, say so */
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
                /* waitpid () failed */
                throw std::runtime_error (strerror (errno));
            }

            /* Keep going until it exited */
        } while (!WIFEXITED (status) && !WIFSIGNALED (status));

        /* Return the exit code */
        return WEXITSTATUS (status);
    }
}

class ChildOutputTest :
    public ::testing::Test
{
    public:

        ChildOutputTest () :
            env (environ)
        {
        }

        virtual void SetUp ()
        {
            argv.append (GetExecutable ());
        }

        virtual std::string GetExecutable () const = 0;

    protected:

        std::vector <std::string> GetChildOutput ();

        ycom::NullTermArray argv;
        ycom::NullTermArray env;
        std::string         lineBuffer;


        Pipe               childStdoutPipe;
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
    ChildStream streambuf (childStdoutPipe.ReadEnd (),
                           bio::never_close_handle);

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
        launchBinaryAndWaitForReturn (yta::passthrough,
                                      argv,
                                      env,
                                      childStdoutPipe.WriteEnd ());
    });
}

TEST_F (DirectlyExecutePassthrough, ExecuteNoArgsReturns1)
{
    EXPECT_EQ (1,
               launchBinaryAndWaitForReturn (yta::passthrough,
                                             argv,
                                             env,
                                             childStdoutPipe.WriteEnd ()));
}

TEST_F (DirectlyExecutePassthrough, ExecuteWithNoExecArgsReturns1)
{
    std::string const MockArgument ("--ARGUMENT");
    argv.append (MockArgument);

    EXPECT_EQ (1,
               launchBinaryAndWaitForReturn (yta::passthrough,
                                             argv,
                                             env,
                                             childStdoutPipe.WriteEnd ()));
}

TEST_F (DirectlyExecutePassthrough, ExecuteNoArgsOutputHeader)
{
    launchBinaryAndWaitForReturn (yta::passthrough,
                                  argv,
                                  env,
                                  childStdoutPipe.WriteEnd ());

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

    launchBinaryAndWaitForReturn (yta::passthrough,
                                  argv,
                                  env,
                                  childStdoutPipe.WriteEnd ());

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
        std::string ("--") + yconst::YiqiToolOption);
}


class DirectlyExecuteSimpleTest :
    public ChildOutputTest
{
    public:

        DirectlyExecuteSimpleTest ()
        {
            env.removeAnyMatching ([](char const *str) -> bool {
                                       return strncmp (str,
                                                       "PATH=", 5) == 0;
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
        launchBinaryAndWaitForReturn (yta::simpleTest,
                                      argv,
                                      env,
                                      childStdoutPipe.WriteEnd ());
    });
}

TEST_F (DirectlyExecuteSimpleTest, ExecuteWithToolArgsRunningUnderMsg)
{
    argv.append (dashdashYiqiToolOption);
    argv.append (passthroughTool);
    launchBinaryAndWaitForReturn (yta::simpleTest,
                                  argv,
                                  env,
                                  childStdoutPipe.WriteEnd ());

    std::vector <std::string> output (GetChildOutput ());

    EXPECT_THAT (output,
                 Contains (
                     AllOf (
                         HasSubstr (yconst::YiqiRunningUnderHeader),
                                    HasSubstr (passthroughTool))));
}


TEST_F (DirectlyExecuteSimpleTest, ExecuteWithToolArgsOptionPassthroughMsg)
{
    argv.append (dashdashYiqiToolOption);
    argv.append (passthroughTool);
    launchBinaryAndWaitForReturn (yta::simpleTest,
                                  argv,
                                  env,
                                  childStdoutPipe.WriteEnd ());

    std::vector <std::string> output (GetChildOutput ());

    EXPECT_THAT (output,
                 Contains (HasSubstr (ytp::OptionHeader)));
}
