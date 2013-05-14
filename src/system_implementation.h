/*
 * system_api.h:
 * A small abstraction layer on top of the operating system
 * API
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_SYSTEM_IMPLEMENTATION_H
#define YIQI_SYSTEM_IMPLEMENTATION_H

#include <memory>

namespace yiqi
{
    namespace system
    {
        namespace api
        {
            class SystemCalls;
            typedef std::unique_ptr <SystemCalls> SystemCallsUnique;

            SystemCallsUnique MakeUNIXSystemCalls ();
        }
    }
}

#endif // YIQI_SYSTEM_IMPLEMENTATION_H
