/*
 * init.h:
 *
 * Global initialization routines, used to start up yiqi on a particular
 * testing platform.
 *
 * See LICENCE.md for Copyright information
 */
#ifndef YIQI_INIT_H
#define YIQI_INIT_H

namespace testing
{
    class Environment;
}

namespace yiqi
{
    namespace platform
    {
        class Interface;
    }

    void Init (platform::Interface &, int argc, char **argv);
}

#endif
