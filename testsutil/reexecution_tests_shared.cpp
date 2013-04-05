/*
 * reexecution.cpp:
 * Definitions for shared data in the reexecution tests
 *
 * See LICENCE.md for Copyright information
 */

#include "test_util.h"
#include "reexecution_tests_shared.h"

namespace ytestrexec = yiqi::testing::reexecution;
namespace ytest = yiqi::testing;

std::string const ytestrexec::NoInstrumentation ("");
std::string const ytestrexec::MockInstrumentation ("mocktool");
std::string const ytestrexec::MockArgument ("--mock");

std::string const ytestrexec::NoExecutablePath ("");
std::vector <std::string> const ytestrexec::MockExecutablePaths =
{
    std::string ("/mock/executable/path/"),
    std::string ("/executable/mock/path/")
};

std::string const ytestrexec::MultiMockExecutablePath (MockExecutablePaths[0] +
                                                       ":" +
                                                       MockExecutablePaths[1]);

std::vector <std::string> const ytestrexec::InitialMockArgv =
{
};

namespace
{
    std::string const MockEnv ("MOCK=mock");
}

char const * const ytestrexec::ProvidedEnvironment[] =
{
    MockEnv.c_str (),
    NULL
};
