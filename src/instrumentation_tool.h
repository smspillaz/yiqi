/*
 * instrumentation_tools_available.h:
 * Provides factory functions for the various implementations
 * of yiqi::instrumentation::tools::Tool available without
 * actually exposing the definition of those tools
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_INSTRUMENTATION_TOOL_H
#define YIQI_INSTRUMENTATION_TOOL_H

#include <string>
#include <memory>

#include <constants.h>

namespace yiqi
{
    namespace instrumentation
    {
	namespace tools
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
	    class Tool
	    {
		public:

		    typedef std::unique_ptr <Tool> Unique;
		    typedef yiqi::constants::InstrumentationTool ToolID;

		    virtual ~Tool () {};

		    /**
		     * @brief InstrumentationWrapper
		     * @return The wrapper program used for instrumentation
		     */
		    virtual std::string const & InstrumentationWrapper () const = 0;

		    /**
		     * @brief InstrumentationName
		     * @return The name of the instrumentation that we are
		     * running under, which may be a specific tool of the
		     * instrumentation wrapper
		     */
		    virtual std::string const & InstrumentationName () const = 0;

		    /**
		     * @brief WrapperOptions
		     * @return Options to pass to that wrapper
		     */
		    virtual std::string const & WrapperOptions () const = 0;

		    /**
		     * @brief ToolIdentifier
		     * @return Fetches an identifier for the current tool
		     */
		    virtual ToolID ToolIdentifier () const = 0;

		protected:

		    Tool () = default;

		private:

		    Tool (Tool const &) = delete;
		    Tool & operator=(Tool const &) = delete;
	    };

	}
    }
}

#endif // YIQI_INSTRUMENTATION_TOOL_H
