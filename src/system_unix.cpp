/*
 * system_unix.cpp:
 * The UNIX implementation of the system calls interface
 *
 * See LICENCE.md for Copyright information
 */

#include <iostream>
#include <string>
#include <sstream>
#include <system_error>

#include <unistd.h>

#include <boost/tokenizer.hpp>

#include <cpp-subprocess/locatebinary.h>
#include <cpp-subprocess/operating_system.h>

#include "system_api.h"
#include "system_implementation.h"
#include "operating_system_wrapper.h"
#include "operating_system_implementation.h"

namespace ps = polysquare::subprocess;
namespace ysysapi = yiqi::system::api;
namespace ysysunix = yiqi::system::unix;

namespace
{
    class UNIXCalls :
        public ysysapi::SystemCalls
    {
        public:

            typedef std::unique_ptr<ysysunix::OperatingSystemWrapper> OSWrapper;
            typedef ps::OperatingSystem::Unique SubprocessOSWrapper;

            UNIXCalls (OSWrapper           userspace,
                       SubprocessOSWrapper subprocessOS);

        private:

            std::string LocateBinary (std::string const &binary) const;
            void ExecInPlace (char const         *binary,
                              char const * const *argv,
                              char const * const *environ) const;
            std::string GetExecutablePath () const;
            char const * const * GetSystemEnvironment () const;

            OSWrapper           userspace;
            SubprocessOSWrapper subprocessOS;
    };
}

UNIXCalls::UNIXCalls (OSWrapper           userspace,
                      SubprocessOSWrapper subprocessOS) :
    userspace (std::move (userspace)),
    subprocessOS (std::move (subprocessOS))
{
}

std::string
UNIXCalls::LocateBinary (std::string const &binary) const
{
    std::string pathString (getenv ("PATH"));
    boost::char_separator <char> sep (":");
    boost::tokenizer <boost::char_separator <char> > paths (pathString, sep);

    std::string executable (ps::locateBinary (binary,
                                              paths,
                                              *subprocessOS));

    return executable;
}

void
UNIXCalls::ExecInPlace (char const         *binary,
                        char const * const *argv,
                        char const * const *env) const
{
    int result = userspace->execve (binary,
                                    argv,
                                    env);

    if (result == -1)
        throw std::system_error (std::error_code (errno,
                                                  std::system_category ()));
    else
        throw std::runtime_error ("execvp failed, but did not return -1");
}

std::string
UNIXCalls::GetExecutablePath () const
{
    char const *path = userspace->getenv ("PATH");
    if (path)
        return std::string (path);

    return std::string ();
}

char const * const *
UNIXCalls::GetSystemEnvironment () const
{
    return const_cast <char const * const *> (userspace->environment ());
}

ysysapi::SystemCalls::Unique
ysysapi::MakeUNIXSystemCalls ()
{
    auto yiqiOS (ysysunix::MakeOSWrapper ());
    auto subprocessOS (ps::MakeOperatingSystem ());
    auto unixCalls (new UNIXCalls (std::move (yiqiOS),
                                   std::move (subprocessOS)));
    return ysysapi::SystemCalls::Unique (unixCalls);
}
