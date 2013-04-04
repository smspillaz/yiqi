/*
 * reexecution.cpp:
 * Provides application logic behind finding the new process to
 * launch and launching it as appropriate
 *
 * See LICENCE.md for Copyright information
 */

#include "commandline.h"
#include "instrumentation_tool.h"
#include "reexecution.h"
#include "system_api.h"
#include "systempaths.h"

namespace ycom = yiqi::commandline;
namespace yexec = yiqi::execution;
namespace ysys = yiqi::system;

void yexec::RelaunchIfNecessary (ycom::ArgvVector const &com,
                                 Tool const             &tool,
                                 SystemCalls const      &system)
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

        char const * execArgv[com.size () + 1];

        /* It must be null-terminated */
        for (size_t i = 0; i < com.size (); ++i)
            execArgv[i] = com[i];

        execArgv[com.size ()] = NULL;

        /* Program ends here in the normal case */
        system.ExecInPlace (executable.c_str (),
                            execArgv);
    }
}
