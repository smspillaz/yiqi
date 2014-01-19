/*
 * system_api_mock.h:
 * A Google Mock implementation of yiqi::system::api::SystemCalls
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_SYSTEM_API_MOCK_H
#define YIQI_SYSTEM_API_MOCK_H

#include <string>

#include <gmock/gmock.h>

#include "system_api.h"

namespace yiqi
{
    namespace mock
    {
        namespace system
        {
            namespace api
            {
                class SystemCalls :
                    public yiqi::system::api::SystemCalls
                {
                    public:

                        SystemCalls ();
                        ~SystemCalls ();

                        void IgnoreCalls ();

                        MOCK_CONST_METHOD1 (LocateBinary,
                                            std::string (std::string const &));
                        MOCK_CONST_METHOD3 (ExecInPlace,
                                            void (char const         *,
                                                  char const * const *,
                                                  char const * const *));
                        MOCK_CONST_METHOD0 (GetExecutablePath, std::string ());
                        MOCK_CONST_METHOD0 (GetSystemEnvironment,
                                            char const * const * ());
                };
            }
        }
    }
}

#endif // YIQI_SYSTEM_API_MOCK_H
