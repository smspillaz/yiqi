/*
 * system_api.h:
 * A small abstraction layer on top of the operating system
 * API
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_SYSTEM_API_H
#define YIQI_SYSTEM_API_H

#include <memory>

namespace yiqi
{
    namespace system
    {
	namespace api
	{
	    class SystemCalls
	    {
		public:

		    typedef std::unique_ptr <SystemCalls> Unique;

		    virtual ~SystemCalls () {};

		    /**
		     * @brief ExeExists checks whether not an executable file
		     * exists at a named path
		     * @param file the path to check
		     * @return true if the file exists, false if not
		     */
		    virtual bool ExeExists (std::string const &file) const = 0;

		    /**
		     * @brief ExecInPlace replaces the current process image
		     * and throws if the operation fails
		     * @param binary the fully-qualified binary path to
		     * execute
		     * @param a arguments to pass to the binary
		     * @throws std::system_error if execvp failed
		     */
		    virtual void ExecInPlace (char const         *binary,
					      char const * const *a) const = 0;

		    /**
		     * @brief GetExecutablePath
		     * @return a colon-delimited set of executable paths
		     */
		    virtual std::string GetExecutablePath () const = 0;

		protected:

		    SystemCalls () = default;

		private:

		    SystemCalls (SystemCalls const &) = delete;
		    SystemCalls & operator=(SystemCalls const &) = delete;
	    };
	}
    }
}

#endif // YIQI_SYSTEM_API_H
