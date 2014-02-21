/*
 * operating_system_implementation.h:
 * Factory function for constructing a ysysunix::OperatingSystemWrapper
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_OPERATING_SYSTEM_IMPLEMENTATION_H
#define YIQI_OPERATING_SYSTEM_IMPLEMENTATION_H

#include <memory>

namespace yiqi
{
    namespace system
    {
        namespace unix
        {
            class OperatingSystemWrapper;

            /* Constructs an operating-system specific abstraction
             * of OperatingSystemWrapper, which is implemented
             * in the operating system specific .cpp file */
            std::unique_ptr <OperatingSystemWrapper> MakeOSWrapper ();
        }
    }
}

#endif
