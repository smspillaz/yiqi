/*
 * reexecution.h:
 * Provides application logic behind finding the new process to
 * launch and launching it as appropriate
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_REEXECUTION_H
#define YIQI_REEXECUTION_H

#include <functional>
#include <vector>

namespace yiqi
{
    namespace commandline
    {
        typedef std::vector <const char *> ArgvVector;
        class NullTermArray;
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
        typedef commandline::NullTermArray NullTermArray;
        typedef system::api::SystemCalls SystemCalls;

        /**
         * @brief FindExecutable
         * @param tool a yiqi::instrumentation::tools::Tool
         * @param calls a set of yiqi::system::api::SystemCalls
         * @throw std::logic_error if @param tool is empty
         * @throw std::runtime_error if the system executable path
         * is empty, or if @param tool could not be found in the
         * system executable paths
         * @return the full path to the executable
         */
        std::string FindExecutable (Tool const        &tool,
                                    SystemCalls const &calls);

        /**
         * @brief GetToolArgv
         * @param tool a yiqi::instrumentation::tools::Tool
         * @param currentArgc the current program argv, passed to main ()
         * @param currentArgv the current program argv, passed to main ()
         * @return a yiqi::commandline::NullTermArray of the argv to
         * pass to the tool executable
         */
        NullTermArray GetToolArgv (Tool const        &tool,
                                   int                currentArgc,
                                   char const * const *currentArgv);

        /**
         * @brief GetToolEnv
         * @param tool a yiqi::instrumentation::tools::Tool
         * @param system a yiqi::system::api::SystemCalls
         * @return a yiqi::commandline::NullTermArray of the environment
         * to pass to the tool executable
         */
        NullTermArray GetToolEnv (Tool const        &tool,
                                  SystemCalls const &system);

        typedef std::function <std::string (Tool const        &,
                                            SystemCalls const &)> FetchExecFunc;
        typedef std::function <NullTermArray (Tool const &)> FetchArgvFunc;
        typedef std::function <NullTermArray (Tool const        &,
                                              SystemCalls const &)> FetchEnvFunc;

        /**
         * @brief Relaunch
         * @param tool a yiqi::instrumentation::tools::Tool with information
         * about what process we should relaunch under
         * @param fetchExecutable a FetchExecFunc callback to fetch the
         * path to the filesystem binary from a
         * yiqi::instrumentation::tools::Tool and
         * yiqi::system::api::SystemCalls
         * @param fetchArgv a FetchArgvFunc callback to fetch the argv
         * to provide to the executed to the tool binary
         * @param fetchEnv a FetchEnvFunc callback to fetch the environment
         * to provide to the executed to the tool binary
         * @throws std::runtime_error if the binary wasn't found
         * @throws std::logic_error if this tool has no binary
         * @throws std::system_error if the system call failed
         * @throws std::exception in other exception cases
         */
        void Relaunch (Tool const           &tool,
                       FetchExecFunc const  &fetchExecutable,
                       FetchArgvFunc const  &fetchArgv,
                       FetchEnvFunc const   &fetchEnv,
                       SystemCalls const    &system);

        /**
         * @brief RelaunchCurrentProgram
         * @param tool a yiqi::instrumentation::tools::Tool with information
         * about what process we should relaunch under
         * @param currentArgc the current program argc passed to main ()
         * @param currentArgv the current program argv passed to main ()
         * @throws std::runtime_error if the binary wasn't found
         * @throws std::logic_error if this tool has no binary
         * @throws std::system_error if the system call failed
         * @throws std::exception in other exception cases
         */
        void RelaunchCurrentProgram (Tool const           &tool,
                                     int                  currentArgc,
                                     char const * const * currentArgv,
                                     SystemCalls const    &system);

    }
}

#endif
