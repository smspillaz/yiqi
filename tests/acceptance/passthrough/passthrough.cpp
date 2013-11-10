/*
 * passthrough.cpp
 * A simple passthrough binary.
 *
 * Prints "PASSTHROUGH BINARY LAUNCHED\n" on startup
 *
 * Takes the first non-option argv[n], forks, and executes it with
 * &(argv[argc - n]) as the child argv. Prints any option argv
 * to stdout as:
 *  "OPTION: %s\n", argv[i]
 *
 * Returns 1 if no executable argv was found. Otherwise returns
 * the child return code.
 *
 * See LICENCE.md for Copyright information
 */

#include <iostream>
#include <sstream>
#include <system_error>

#include <unistd.h>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>

#include <operating_system_implementation.h>
#include <operating_system_wrapper.h>

#include "passthrough_constants.h"

namespace ysysunix = yiqi::system::unix;
namespace ytp = yiqi::testing::passthrough;

namespace
{
    std::string const dashdash ("--");
}

int main (int argc, char *argv[])
{
    std::cout << ytp::StartupMessage << std::endl;

    for (int i = 1; i < argc; ++i)
    {
        if (strncmp (argv[i],
                     dashdash.c_str (),
                     dashdash.size ()) == 0)
            std::cout << ytp::OptionHeader
                      << std::string (argv[i])
                      << std::endl;
        else
        {
            pid_t child = fork ();

            /* Child process */
            if (child == 0)
            {
                auto os = ysysunix::MakeOSWrapper ();

                if (os->execve (argv[i],
                                &argv[i],
                                os->environment ()) == -1)
                {
                    std::error_code code (errno,
                                          std::system_category ());
                    throw std::system_error (code);
                }
            }
            /* Parent process, error */
            else if (child == -1)
            {
                std::error_code code (errno,
                                      std::system_category ());
                throw std::system_error (code);
            }
            /* Parent process */
            else
            {
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

                /* Return child return code */
                return WEXITSTATUS (status);
            }
        }
    }

    return 1;
}
