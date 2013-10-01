/*
 * instrumentation_tools_available.h:
 * Provides factory functions for the various implementations
 * of yiqi::instrumentation::tools::Tool available without
 * actually exposing the definition of those tools
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_INSTRUMENTATION_TOOLS_AVAILABLE_H
#define YIQI_INSTRUMENTATION_TOOLS_AVAILABLE_H

#include <memory>

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            class Program;
            typedef std::unique_ptr <Program> ToolUniquePtr;

            ToolUniquePtr MakeNoneTool ();
            ToolUniquePtr MakeTimerTool ();
            ToolUniquePtr MakeMemcheckTool ();
            ToolUniquePtr MakeCallgrindTool ();
            ToolUniquePtr MakeCachegrindTool ();
            ToolUniquePtr MakePassthroughTool ();
        }
    }
}

#endif // YIQI_INSTRUMENTATION_TOOLS_AVAILABLE_H
