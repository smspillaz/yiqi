/*
 * commandline.h:
 * Definitions of a function to extract a command line
 * from the currently running program argv and also the
 * instrumentation parameters
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_COMMANDLINE_H
#define YIQI_COMMANDLINE_H

#include <memory>
#include <string>

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            class Tool;
            typedef std::unique_ptr <Tool> ToolUniquePtr;
        }
    }

    namespace commandline
    {
        std::string BuildCommandLine (int                 argc,
                                      char const * const  *argv,
                                      ToolUniquePtr const &tool);
    }
}

#endif // YIQI_COMMANDLINE_H
