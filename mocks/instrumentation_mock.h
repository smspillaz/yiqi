/*
 * instrumentation_mock.h:
 * A Google Mock implementation of yiqi::instrumentation::tools::Tool
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_INSTRUMENTATION_MOCK_H
#define YIQI_INSTRUMENTATION_MOCK_H

#include <string>

#include <gmock/gmock.h>

#include "instrumentation_tool.h"

namespace yiqi
{
    namespace mock
    {
        namespace instrumentation
        {
            namespace tools
            {
                class Tool :
                    public yiqi::instrumentation::tools::Tool
                {
                    public:

                        Tool ();
                        ~Tool ();

                        MOCK_CONST_METHOD0 (InstrumentationWrapper,
                                            std::string const & ());
                        MOCK_CONST_METHOD0 (WrapperOptions,
                                            std::string const & ());
                        MOCK_CONST_METHOD0 (ToolIdentifier,
                                            ToolID ());
                };
            }
        }

    }
}

#endif // YIQI_INSTRUMENTATION_MOCK_H
