/*
 * platform_gtest.h:
 *
 * Platform abstraction for the Google Test library.
 *
 * See LICENCE.md for Copyright information
 */
#ifndef YIQI_PLATFORM_GTEST_H
#define YIQI_PLATFORM_GTEST_H

#include <yiqi/platform.h>

namespace yiqi
{
    namespace gtest
    {
        using platform::Controller;

        class Platform :
            public platform::Interface
        {
            public:

                void Register (std::unique_ptr <Controller> &&) override;
        };
    }
}

#endif
