/*
 * reexecution.h:
 * Provides application logic behind finding the new process to
 * launch and launching it as appropriate
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_REEXECUTION_H
#define YIQI_REEXECUTION_H

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            class Tool;
        }
    }

    namespace system
    {
        namespace api
        {
            class SystemCalls;
        }
    }

    namespace execution
    {
        typedef instrumentation::tools::Tool Tool;
        typedef system::api::SystemCalls SystemCalls;
        void RelaunchIfNecessary (int                argc,
                                  char const * const *argv,
                                  Tool const         &tool,
                                  SystemCalls const  &system);

    }
}

#endif
