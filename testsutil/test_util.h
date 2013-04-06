/*
 * test_util.h:
 * Utility functions and wrappers shared amonst testing
 * translation units
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_TEST_UTIL_H
#define YIQI_TEST_UTIL_H

#include <stdexcept>
#include <tuple>
#include <vector>
#include <string>

namespace yiqi
{
    namespace testing
    {
        /**
         * @brief Copy array into vector so we
         * can use it with Google Test matchers
         */
        template <typename T>
        std::vector <T> ToVector (T const *array, size_t size)
        {
            if (size)
                return std::vector <T> (array, array + size);
            else
                throw std::out_of_range ("ToVector: size is < 1");
        }

        /**
         * @brief A tuple type with a managed array of arguments
         */
        typedef std::tuple <int, std::vector <char const *> > CommandLineArguments;

        int ArgumentCount (CommandLineArguments const &args);
        char const * const * Arguments (CommandLineArguments const &args);

        /**
	 * @brief MockProgramName the program name used by GenerateCommandLine
         */
	extern char const *MockProgramName;

        /**
         * @brief GenerateCommandLine
         * @return a CommandLineArguments object from the current vector
         * of strings provided, plus a mock program name
         * specified in MockProgramName ()
         */
        CommandLineArguments
        GenerateCommandLine (std::vector <std::string> const &);
    }
}

#endif // YIQI_TEST_UTIL_H
