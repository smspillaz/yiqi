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

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            class Program;
        }
    }

    namespace commandline
    {
        typedef yiqi::instrumentation::tools::Program Tool;
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
         * @brief StringVectorToArgv turns a CommandArguments into a
         * null-terminated vector of char const *, suitable for
         * passing a into the execvp family of system calls
         * @param args a CommandArguments to turn into a const array
         * @return a const vector of strings, suitable for the execvp family
         * of system calls
         */
        typedef std::vector <char const *> ArgvVector;
        ArgvVector const StringVectorToArgv (CommandArguments const &args);

        /**
         * @brief The NullTermArray represents an array of char const *
         * which will always be terminated by a NULL
         * @todo supply a move constructor
         */
        class NullTermArray
        {
            public:

                NullTermArray ();
                ~NullTermArray ();

                /**
                 * @brief NullTermArray
                 * @pre envp must be NULL-terminated
                 * @pre envp may not be NULL
                 * @param envp a null-terminated pointer to a char const *
                 * representing the NullTermArray to manipulate
                 */
                explicit NullTermArray (char const * const *array);
                NullTermArray (NullTermArray const &);
                NullTermArray (NullTermArray &&);
                NullTermArray & operator= (NullTermArray rhs);

                bool operator== (NullTermArray const &rhs) const;
                bool operator!= (NullTermArray const &rhs) const;

                /**
                 * @brief append appends a new std::string const &value to the
                 * end of the NullTermArray, just before the null-terminator. It
                 * will also provide storage for value, so value can be a temporary.
                 * @throws std::out_of_memory if the underlying vector
                 * cannot allocate space for the new value
                 * @param value the value to append
                 */
                void append (std::string const &value);

                typedef std::vector <std::string> StringVector;

                /**
                 * @brief append appends a whole vector of std::string const &value
                 * to the end of the NullTermArray, just before the null-terminator.
                 * It provides storage for each of the specified strings
                 * @throws std::out_of_memory if the underlying vector
                 * cannot allocate space for the new value
                 * @param values the values to append
                 */
                void append (StringVector const &values);

                typedef std::function <bool (char const *)>  RemoveFunc;

                /**
                 * @brief removeAnyMatching calls remover for each member
                 * of the underlying vector and removes any for which
                 * operator (char const *) return true;
                 * @remover a callback to invoke to determine what should
                 * be removed
                 */
                void removeAnyMatching (RemoveFunc const &remover);

                /**
                 * @brief eraseAppended finds a block of appended values in
                 * the vector specified and erases them. This function isn't perfect -
                 * in the exception handling case as it doesn't know how many values
                 * have been appended that it needs to erase. It applies a heuristic
                 * to check.
                 * @param values the block of values to erase
                 */
                void eraseAppended (StringVector const &values);

                /**
                 * @brief underlyingArray
                 * @return the underlying array of char const *
                 */
                char const * const * underlyingArray () const;

                /**
                 * @brief underlyingArrayLen
                 * @return the number of elements in the underlying array
                 * including the null-terminator
                 */
                size_t underlyingArrayLen () const;

                class Private;

                friend void swap (NullTermArray &lhs, NullTermArray &rhs);

            private:

                std::unique_ptr <Private> priv;
        };

        /**
         * @brief InsertEnvironmentPair inserts a new key=value pair into
         * the provided NullTermArray
         * @param array a yiqi::commandline::NullTermArray
         * @param key the KEY value
         * @param value the value
         */
        void InsertEnvironmentPair (NullTermArray &array,
                                    char const    *key,
                                    char const    *value);

        void swap (NullTermArray &lhs, NullTermArray &rhs);
    }
}

#endif // YIQI_COMMANDLINE_H
