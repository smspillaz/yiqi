/*
 * system_unix.cpp:
 * The UNIX implementation of the system calls interface
 *
 * See LICENCE.md for Copyright information
 */

#include <string>
#include <system_error>

#include <unistd.h>

#include "system_api.h"
#include "system_implementation.h"
#include "operating_system_wrapper.h"
#include "operating_system_implementation.h"

namespace ysysapi = yiqi::system::api;
namespace ysysunix = yiqi::system::unix;

namespace
{
    class UNIXCalls :
        public ysysapi::SystemCalls
    {
        public:

            typedef std::unique_ptr<ysysunix::OperatingSystemWrapper> OSWrapper;

            UNIXCalls (OSWrapper userspace);

        private:

            bool ExeExists (std::string const &f) const;
            void ExecInPlace (char const         *binary,
                              char const * const *argv,
                              char const * const *environ) const;
            std::string GetExecutablePath () const;
            char const * const * GetSystemEnvironment () const;

            OSWrapper userspace;
    };
}

UNIXCalls::UNIXCalls (UNIXCalls::OSWrapper userspace) :
    userspace (std::move (userspace))
{
}

bool
UNIXCalls::ExeExists (std::string const &file) const
{
    if (userspace->access (file.c_str (), X_OK) == 0)
        return true;

    return false;
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
    auto os (ysysunix::MakeOSWrapper ());
    return ysysapi::SystemCalls::Unique (new UNIXCalls (std::move (os)));
}
