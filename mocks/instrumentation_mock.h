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
                class Program :
                    public yiqi::instrumentation::tools::Program
                {
                    public:

                        Program ();
                        ~Program ();

                        void IgnoreCalls ();

                        MOCK_CONST_METHOD0 (WrapperBinary,
                                            std::string const & ());
                        MOCK_CONST_METHOD0 (WrapperOptions,
                                            std::string const & ());
                        MOCK_CONST_METHOD0 (Name,
                                            std::string const & ());
                        MOCK_CONST_METHOD0 (ToolIdentifier,
                                            ToolID ());
                };
            }
        }

    }
}

#endif // YIQI_INSTRUMENTATION_MOCK_H
