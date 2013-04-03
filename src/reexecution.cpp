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

namespace yexec = yiqi::execution;

void yexec::RelaunchIfNecessary (int                argc,
                                 char const * const *argv,
                                 Tool const         &tool,
                                 SystemCalls const  &system)
{
}
