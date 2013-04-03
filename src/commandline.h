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
#include <vector>

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            class Tool;
        }
    }

    namespace commandline
    {
        typedef yiqi::instrumentation::tools::Tool Tool;
        /**
         * @brief BuildCommandLine extracts a command line from the provided
         * parameters
         * @param argc the program argc
         * @param argv the program argv
         * @param tool a yiqi::instrumentation::tools::Tool representing the
         * instrumentation that should be performed and the command required
         * @return an std::vector of std::string with the
         * command line to pass to exec ()
         */
        typedef std::vector <std::string> CommandArguments;
        CommandArguments BuildCommandLine (int                 argc,
                                           char const * const  *argv,
                                           Tool const          &tool);

        /**
         * @brief StringVectorToArgv turns a CommandArguments into vector
         * of char const *, suitable for passing a into the execvp family
         * of system calls
         * @param args a CommandArguments to turn into a const array
         * @return a const vector of strings, suitable for the execvp family
         * of system calls
         */
        typedef std::vector <char const *> ArgvVector;
        ArgvVector const StringVectorToArgv (CommandArguments const &args);
    }
}

#endif // YIQI_COMMANDLINE_H
