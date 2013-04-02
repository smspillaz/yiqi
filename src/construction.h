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
    namespace construction
    {
	/**
	 * @brief An interface which describes some of the construction
	 * parameters for the environment, including how the program should
	 * be instrumented and also some details on callbacks into that
	 * instrumentation.
	 *
	 * An instance of this interface is usually constructed by
	 * ParseOptionsToParameters()
	 */
	class Parameters
	{
	    public:

		typedef std::unique_ptr <Parameters> Unique;

		Parameters (Parameters const &) = delete;
		Parameters & operator=(Parameters const &) = delete;
		virtual ~Parameters () = default;

		/**
		 * @brief Fetch the command line used for instrumentation
		 * @return
		 */
		virtual std::string InstrumentationCommand () = 0;

	    protected:

		Parameters () = default;
	};

	/**
	 * @brief ParseOptionsToParameters
	 * @param argc Number of arguments from main()
	 * @param argv Arguments from main()
	 * @param description A boost::program_options::options_description
	 * object which describes which options should be available
	 * @throws A boost::program_options::error on encountering a malformed
	 * or unknown option
	 * @return A yiqi::construction::Parameters::Unique object
	 */
	typedef boost::program_options::options_description Options;
	Parameters::Unique
	ParseOptionsToParameters (int                argc,
				  const char * const *argv,
				  Options const      &description);
    }
}

#endif // YIQI_CONSTRUCTION_H
