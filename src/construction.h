/*
 * construction.h:
 * Provide header declarations for functions to determine
 * how the test environment should be constructed
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_CONSTRUCTION_H
#define YIQI_CONSTRUCTION_H

#include <string>
#include <memory>
#include <boost/program_options.hpp>

#include "instrumentation_tools_available.h"

namespace yiqi
{
    namespace constants
    {
        enum class InstrumentationTool;
    }

    namespace instrumentation
    {
        namespace tools
        {
            class Tool;
        }
    }

    namespace construction
    {
        typedef yiqi::instrumentation::tools::Tool InstrumentationToolCommand;
        typedef std::unique_ptr <InstrumentationToolCommand> ToolUniquePtr;

        /**
         * @brief FetchOptionsDescription returns the
         * available command line options to be used with this program
         * @return a boost::program_options::options_description object
         * describing how the command line options should be parsed
         */
        boost::program_options::options_description
        FetchOptionsDescription ();

        typedef boost::program_options::options_description Options;

        /**
         * @brief ParseOptionsForTool
         * @brief ParseOptionsToParameters
         * @param argc Number of arguments from main()
         * @param argv Arguments from main()
         * @param description A boost::program_options::options_description
         * object which describes which options should be available
         * @throws A boost::program_options::error on encountering a malformed
         * or unknown option
         * @return An std::string with the current instrumentation tool
         */
        std::string
        ParseOptionsForTool (int                argc,
                             const char * const *argv,
                             Options const      &description);

        ToolUniquePtr
        MakeSpecifiedTool (yiqi::constants::InstrumentationTool);

        /**
         * @brief ParseOptionsToParameters
         * @param argc Number of arguments from main()
         * @param argv Arguments from main()
         * @param description A boost::program_options::options_description
         * object which describes which options should be available
         * @throws A boost::program_options::error on encountering a malformed
         * or unknown option
         * @return A yiqi::construction::ToolUniquePtr object
         */
        ToolUniquePtr
        ParseOptionsToToolUniquePtr (int                argc,
                                     const char * const *argv,
                                     Options const      &description);
    }
}

#endif // YIQI_CONSTRUCTION_H
