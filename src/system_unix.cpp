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
                              char const * const *a) const;
            std::string GetExecutablePath () const;
    };
}

bool
UNIXCalls::ExeExists (std::string const &file) const
{
    if (access (file.c_str (), X_OK))
        return true;

    return false;
}

void
UNIXCalls::ExecInPlace (char const         *binary,
                        char const * const *a) const
{
    int result = execvp (binary, const_cast <char * const *> (a));

    if (result == -1)
        throw std::system_error (std::error_code (errno,
                                                  std::system_category ()));
    else
        throw std::runtime_error ("execvp failed, but did not return -1");
}

std::string
UNIXCalls::GetExecutablePath () const
{
    char const *path = getenv ("path");
    if (path)
        return std::string (path);

    return std::string ();
}
