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
//#include <valgrind/memcheck.h>
//#include <valgrind.h>

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            /**
             * @brief An interface which describes some of the construction
             * parameters for the environment, including how the program should
             * be instrumented.
             *
             * An instance of this interface is usually constructed by
             * ParseOptionsToParameters()
             */
            class Program
            {
                public:

                    typedef std::unique_ptr <Program> Unique;
                    typedef constants::InstrumentationTool ToolID;

                    virtual ~Program () {};

                    /**
                     * @brief WrapperBinary
                     * @return The wrapper program used for instrumentation
                     */
                    virtual std::string const & WrapperBinary () const = 0;

                    /**
                     * @brief Name
                     * @return The name of the instrumentation that we are
                     * running under, which may be a specific tool of the
                     * instrumentation wrapper
                     */
                    virtual std::string const & Name () const = 0;

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

                    Program () = default;

                private:

                    Program (Program const &) = delete;
                    Program & operator= (Program const &) = delete;
            };

            /**
             * @brief An interface for interacting with the currently
             * instrumenting tool, namely as a means to start and stop
             * instrumentation
             */
            class Controller
            {
                public:

                    typedef std::unique_ptr <Controller> Unique;
                    typedef constants::InstrumentationTool ToolID;

                    enum class FinishMode
                    {
                        Report = 0,
                        Throw
                    };

                    virtual ~Controller () {};

                    /**
                     * @brief Start collecting instrumentation data from
                     * the currently active tool
                     */
                    virtual void Start () = 0;

                    /**
                     * @brief Stop collecting instrumentation data from
                     * the currently active tool.
                     * @param mode What to do when instrumentation data has
                     * been collected. If mode is Throw, then any data
                     * which would almost always be considered a negative will
                     * result in an exception being thrown, failing the test.
                     */
                    virtual void Stop (FinishMode mode) = 0;

                    /**
                     * @brief ToolIdentifier
                     * @return Fetches an identifier for the current tool
                     */
                    virtual ToolID ToolIdentifier () const = 0;

                protected:

                    Controller () = default;

                private:

                    Controller (Controller const &) = delete;
                    Controller & operator= (Controller const &) = delete;
            };

            /**
             * @brief A bundle of factory functions for a particular tool each
             * describing a different interface for working with that tool but
             * returning the abstract interface for it
             */
            struct FactoryPackage
            {
                typedef Program::Unique (*MakeProgram) ();
                typedef Controller::Unique (*MakeController) ();

                MakeProgram program;
                MakeController controller;
            };
        }
    }
}

#endif // YIQI_INSTRUMENTATION_TOOL_H
