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

        /**
         * @brief The Environment value represents a set of program environment
         * variables, suitable for passing to execvpe ()
         */
        class Environment
        {
            public:

                Environment ();
                ~Environment ();

                /**
                 * @brief Environment creates a representation of the passed
                 * envp as an indepedent copy of those environment variables
                 * @pre envp must be NULL-terminated
                 * @pre envp may not be NULL
                 * @param envp a null-terminated pointer to a char const *
                 * representing the environment to manipulate
                 */
                explicit Environment (char const * const *envp);
                Environment (Environment const &);
                Environment & operator= (Environment rhs);

                bool operator== (Environment const &rhs) const;
                bool operator!= (Environment const &rhs) const;

                /**
                 * @brief insert inserts a new variable value pair
                 * @param variable
                 * @param value
                 * @throws std::out_of_memory if the underlying vector
                 * cannot allocate space for the new variable-value pair
                 */
                void insert (const char *variable,
                             const char *value);

                /**
                 * @brief underlyingEnvironmentArray
                 * @return the underlying null-terminated list of environment
                 * variables suitable for passing to execvpe ()
                 */
                char const * const * underlyingEnvironmentArray () const;

                /**
                 * @brief underlyingEnvironmentArrayLen
                 * @return the number of elements in the underlying array
                 * including the null-terminator
                 */
                size_t underlyingEnvironmentArrayLen () const;

                class Private;

                friend void swap (Environment &lhs, Environment &rhs);

            private:

                std::unique_ptr <Private> priv;
        };

        void swap (Environment &lhs, Environment &rhs);
    }
}

#endif // YIQI_COMMANDLINE_H
