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

#include <unistd.h>

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
                 FetchEnFunc const    &fetchEnv,
                 SystemCalls const    &system)
{
    std::string const   executable (fetchExecutable (tool, system));
    ycom::NullTermArray argv (fetchArgv (tool));
    ycom::NullTermArray env (fetchEnv (tool, system));

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
    FetchEnFunc fetchEnv (std::bind (yexec::GetToolEnv, _1, _2));

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
    std::string const &wrapper (tool.WrapperBinary ());

    if (wrapper.empty ())
        throw std::logic_error ("provided a Tool "
                                "with no WrapperBinary");

    return system.LocateBinary (wrapper);
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
    std::string const &name (tool.Name ());
    ycom::NullTermArray environment (system.GetSystemEnvironment ());

    if (!name.empty ())
        ycom::InsertEnvironmentPair (environment,
                                     yconst::YiqiToolEnvKey,
                                     name.c_str ());
    else
        throw std::logic_error ("provided tool with no Name");

    return environment;
}
