/*
 * instrumentation_tool_valgrind_base.h:
 * Provides a template for building instrumentation tools based
 * on valgrind
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_INSTRUMENTATION_TOOL_VALGRIND_BASE_H
#define YIQI_INSTRUMENTATION_TOOL_VALGRIND_BASE_H

#include "instrumentation_tool.h"

namespace yiqi
{
    namespace instrumentation
    {
        namespace tools
        {
            namespace valgrind
            {
                class ProgramBase :
                    public Program
                {
                    protected:

                        ProgramBase () = default;

                    private:

                        std::string const & WrapperBinary () const override;
                        std::string const & WrapperOptions () const override;
                        std::string const & Name () const override;

                        virtual std::string const &
                        ToolAdditionalOptions () const = 0;
                };
            }
        }
    }
}

#endif // YIQI_INSTRUMENTATION_TOOL_VALGRIND_BASE_H
