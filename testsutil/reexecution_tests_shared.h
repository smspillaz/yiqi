/*
 * reexecution.h:
 * Declarations for shared data in the re-execution tests
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_REEXECUTION_TESTS_SHARED_H
#define YIQI_REEXECUTION_TESTS_SHARED_H

#include <string>

namespace yiqi
{
    namespace testing
    {
        namespace reexecution
        {
            extern std::string const NoInstrumentation;
            extern std::string const MockInstrumentation;
            extern std::string const MockArgument;

            extern std::string const NoExecutablePath;
            extern std::vector <std::string> const MockExecutablePaths;
            extern std::string const MultiMockExecutablePath;

            extern std::vector <std::string> const InitialMockArgv;

            extern char const * const ProvidedEnvironment[];
        }
    }
}

#endif // YIQI_REEXECUTION_TESTS_SHARED_H
