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
		     * @param argv arguments to pass to the binary
		     * @param e a pointer to a null-terminated array of
		     * char const * of system environment variables with
		     * the format KEY=value
		     * @throws std::system_error if execvp failed
		     */
		    virtual void ExecInPlace (char const         *binary,
					      char const * const *argv,
					      char const * const *e) const = 0;

		    /**
		     * @brief GetExecutablePath
		     * @return a colon-delimited set of executable paths
		     */
		    virtual std::string GetExecutablePath () const = 0;

		    /**
		     * @brief GetSystemEnvironment
		     * @return a null-terminated array of strings representing the
		     * current process environment, with the format KEY=value
		     */
		    virtual char const * const * GetSystemEnvironment () const = 0;

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
