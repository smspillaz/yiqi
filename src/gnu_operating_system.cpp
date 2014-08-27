/*
 * gnu_operating_system.cpp
 *
 * Thin noexcept wrapper for some UNIX functionality and quirks on
 * Darwin (a.k.a Mac OS X)
 *
 * See LICENCE.md for Copyright information.
 */

#include "nonportable_gnu.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "operating_system_wrapper.h"
#include "operating_system_implementation.h"

namespace ysysunix = yiqi::system::unix;

namespace
{
    class GNUOperatingSystem :
        public ysysunix::OperatingSystemWrapper
    {
        public:

            int access (char const *path, int flags) noexcept;
            int execve (char const         *path,
                        char const * const *argv,
                        char const * const *env) noexcept;
            char const * getenv (char const *env) noexcept;
            char const * const * environment () noexcept;
            int pipe (int p[2]) noexcept;
    };
}

int
GNUOperatingSystem::access (char const *path, int flags) noexcept
{
    return ::access (path, flags);
}

int
GNUOperatingSystem::execve (char const         *path,
                            char const * const *argv,
                            char const * const *env) noexcept
{
    return ::execvpe (path,
                      const_cast <char * const *> (argv),
                      const_cast <char * const *> (env));
}

char const *
GNUOperatingSystem::getenv (char const *env) noexcept
{
    return ::getenv (env);
}

char const * const *
GNUOperatingSystem::environment () noexcept
{
    return environ;
}

int
GNUOperatingSystem::pipe (int p[2]) noexcept
{
    return pipe2 (p, O_CLOEXEC);
}

std::unique_ptr <ysysunix::OperatingSystemWrapper>
ysysunix::MakeOSWrapper ()
{
    return std::unique_ptr <ysysunix::OperatingSystemWrapper> (
        new GNUOperatingSystem ());;
}
