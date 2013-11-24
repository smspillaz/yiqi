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
            struct FactoryPackage;
        }
    }

    namespace construction
    {
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
         * @brief ParseOptionsForToolName
         * @param argc Number of arguments from main()
         * @param argv Arguments from main()
         * @param description A boost::program_options::options_description
         * object which describes which options should be available
         * @throws A boost::program_options::error on encountering a malformed
         * or unknown option
         * @return An std::string with the current instrumentation tool
         */
        std::string
        ParseOptionsForToolName (int                argc,
                                 const char * const *argv,
                                 Options const      &description);

        /**
         * @brief FactoryPackageForTool returns a set of factory functions
         * for a named yiqi::constants::InstrumentationTool
         * @param id A yiqi::constants::InstrumentationTool
         * @throws An std::exception if the specified tool does not exist or
         * is out of range. This should be considered a fatal error.
         * @return An instrumentation::tools::FactoryPackage
         */
        instrumentation::tools::FactoryPackage const &
        FactoryPackageForTool (yiqi::constants::InstrumentationTool id);
    }
}

#endif // YIQI_CONSTRUCTION_H
