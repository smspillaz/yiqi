/*
 * system_unix.cpp:
 * The UNIX implementation of the system calls interface
 *
 * See LICENCE.md for Copyright information
 */

#include <system_error>

#include <unistd.h>

#include "system_api.h"
#include "system_implementation.h"

namespace ysysapi = yiqi::system::api;

namespace
{
    class UNIXCalls :
        public ysysapi::SystemCalls
    {
        private:

            bool ExeExists (std::string const &f) const;
            void ExecInPlace (char const         *binary,
                              char const * const *argv,
                              char const * const *environ) const;
            std::string GetExecutablePath () const;
            char const * const * GetSystemEnvironment () const;
    };
}

bool
UNIXCalls::ExeExists (std::string const &file) const
{
    if (access (file.c_str (), X_OK) == 0)
        return true;

    return false;
}

void
UNIXCalls::ExecInPlace (char const         *binary,
                        char const * const *argv,
                        char const * const *env) const
{
    int result = execvpe (binary,
                          const_cast <char * const *> (argv),
                          const_cast <char * const *> (env));

    if (result == -1)
        throw std::system_error (std::error_code (errno,
                                                  std::system_category ()));
    else
        throw std::runtime_error ("execvp failed, but did not return -1");
}

std::string
UNIXCalls::GetExecutablePath () const
{
    char const *path = getenv ("PATH");
    if (path)
        return std::string (path);

    return std::string ();
}

char const * const *
UNIXCalls::GetSystemEnvironment () const
{
    return const_cast <char const * const *> (environ);
}

ysysapi::SystemCalls::Unique
ysysapi::MakeUNIXSystemCalls ()
{
    return ysysapi::SystemCalls::Unique (new UNIXCalls ());
}
