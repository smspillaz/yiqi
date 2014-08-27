/*
 * environment.h:
 *
 * Factory functions for a ::testing::Environment *
 * that can be passed to ::testing::AddGlobalTestEnvironment
 *
 * See LICENCE.md for Copyright information
 */
#ifndef YIQI_PLATFORM_H
#define YIQI_PLATFORM_H

#include <memory>

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            class Controller;
        }
    }

    namespace platform
    {
        typedef instrumentation::tools::Controller Controller;

        class Interface
        {
            public:

                Interface () = default;
                virtual ~Interface () {};

                virtual void Register (std::unique_ptr <Controller> &&) = 0;

            private:

                Interface (Interface const &) = delete;
                Interface & operator= (Interface const &) = delete;
        };
    }
}

#endif
