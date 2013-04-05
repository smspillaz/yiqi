/*
 * reexecution.cpp:
 * Provides application logic behind finding the new process to
 * launch and launching it as appropriate
 *
 * See LICENCE.md for Copyright information
 */

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "commandline.h"
#include "constants.h"
#include "instrumentation_tool.h"
#include "reexecution.h"
#include "system_api.h"
#include "systempaths.h"

namespace yconst = yiqi::constants;
namespace ycom = yiqi::commandline;
namespace yexec = yiqi::execution;
namespace ysys = yiqi::system;

void
yexec::Relaunch (Tool const           &tool,
                 FetchExecFunc const  &fetchExecutable,
                 FetchArgvFunc const  &fetchArgv,
                 FetchEnvFunc const   &fetchEnv,
                 SystemCalls const    &system)
{
    std::string const   executable (fetchExecutable (tool, system));
    ycom::NullTermArray argv (fetchArgv (tool));
    ycom::NullTermArray env  (fetchEnv (tool, system));

    system.ExecInPlace (executable.c_str (),
                        argv.underlyingArray (),
                        env.underlyingArray ());
}

void
yexec::RelaunchCurrentProgram (Tool const           &tool,
                               int                  currentArgc,
                               char const * const * currentArgv,
                               SystemCalls const    &system)
{
    using namespace std::placeholders;

    FetchExecFunc fetchExecutable (std::bind (yexec::FindExecutable, _1, _2));
    FetchArgvFunc fetchArgv (std::bind (yexec::GetToolArgv, _1,
                                        currentArgc, currentArgv));
    FetchEnvFunc fetchEnv (std::bind (yexec::GetToolEnv, _1, _2));

    Relaunch (tool,
              fetchExecutable,
              fetchArgv,
              fetchEnv,
              system);
}

std::string
yexec::FindExecutable (Tool const        &tool,
                       SystemCalls const &system)
{
    std::string const &wrapper (tool.InstrumentationWrapper ());

    if (wrapper.empty ())
        throw std::logic_error ("provided a Tool "
                                "with no InstrumentationWrapper");

    std::string execPath (system.GetExecutablePath ());

    if (execPath.empty ())
        throw std::runtime_error ("system executable path is empty");

    auto execPaths (ysys::SplitPathString (execPath.c_str ()));

    for (std::string const &path : execPaths)
    {
        std::string const executable (path + "/" + wrapper);

        if (system.ExeExists (executable))
            return executable;
    }

    std::stringstream ss;
    ss << "Could not find the executable " << wrapper
       << " anywhere in your executable path" << std::endl
       << "Searched the following paths: " << std::endl;

    for (std::string const &path : execPaths)
        ss << " - " << path << std::endl;

    throw std::runtime_error (ss.str ());
}

ycom::NullTermArray
yexec::GetToolArgv (Tool const        &tool,
                    int                currentArgc,
                    char const * const *currentArgv)
{
    ycom::CommandArguments args (ycom::BuildCommandLine (currentArgc,
                                                         currentArgv,
                                                         tool));

    /* We have to insert everything maually as NullTermArray must
     * allocate storage for the new strings */
    ycom::NullTermArray array;
    array.append (args);

    return array;
}

ycom::NullTermArray
yexec::GetToolEnv (Tool const        &tool,
                   SystemCalls const &system)
{
    std::string const &wrapper (tool.InstrumentationWrapper ());
    ycom::NullTermArray environment (system.GetSystemEnvironment ());

    if (!wrapper.empty ())
        ycom::InsertEnvironmentPair (environment,
                                     yconst::YiqiToolEnvKey (),
                                     wrapper.c_str ());

    return environment;
}
