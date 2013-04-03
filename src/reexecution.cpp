/*
 * reexecution.cpp:
 * Provides application logic behind finding the new process to
 * launch and launching it as appropriate
 *
 * See LICENCE.md for Copyright information
 */

#include "instrumentation_tool.h"
#include "reexecution.h"
#include "system_api.h"
#include "systempaths.h"

namespace yexec = yiqi::execution;
namespace ysys = yiqi::system;

/* TODO: the argc and argv here should be for the launched process not the original one */
void yexec::RelaunchIfNecessary (int                argc,
                                 char const * const *argv,
                                 Tool const         &tool,
                                 SystemCalls const  &system)
{
    std::string const &wrapper (tool.InstrumentationWrapper ());

    if (wrapper.empty ())
        return;

    std::string execPath (system.GetExecutablePath ());

    if (execPath.empty ())
        return;

    auto        execPaths (ysys::SplitPathString (execPath.c_str ()));

    for (std::string const &path : execPaths)
    {
        std::string const executable (path + "/" + wrapper);

        if (!system.ExeExists (executable))
            continue;

        char const * const execArgv[] =
        {
            wrapper.c_str (),
            argv[0],
            tool.WrapperOptions ().c_str (),
            NULL
        };

        /* Program ends here in the normal case */
        system.ExecInPlace (executable.c_str (),
                            execArgv);
    }
}
