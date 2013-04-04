/*
 * reexecution.h:
 * Provides application logic behind finding the new process to
 * launch and launching it as appropriate
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_REEXECUTION_H
#define YIQI_REEXECUTION_H

#include <vector>

namespace yiqi
{
    namespace commandline
    {
        typedef std::vector <const char *> ArgvVector;
        class Environment;
    }

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

        typedef std::vector <char *> EnvironmentVector;

        void RelaunchIfNecessary (commandline::ArgvVector  const &argv,
                                  commandline::Environment const &env,
                                  Tool const                     &tool,
                                  SystemCalls const              &system);

    }
}

#endif
