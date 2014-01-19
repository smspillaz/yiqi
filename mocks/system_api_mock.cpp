/*
 * system_api_mock.cpp:
 * A Google Mock implementation of yiqi::system::api::SystemCalls
 *
 * See LICENCE.md for Copyright information
 */

#include "system_api_mock.h"

using ::testing::_;
using ::testing::AtLeast;

namespace ymock = yiqi::mock;

ymock::system::api::SystemCalls::SystemCalls ()
{
}

ymock::system::api::SystemCalls::~SystemCalls ()
{
}

void
ymock::system::api::SystemCalls::IgnoreCalls ()
{
    EXPECT_CALL (*this, LocateBinary (_)).Times (AtLeast (0));
    EXPECT_CALL (*this, ExecInPlace (_, _, _)).Times (AtLeast (0));
    EXPECT_CALL (*this, GetExecutablePath ()).Times (AtLeast (0));
    EXPECT_CALL (*this, GetSystemEnvironment ()).Times (AtLeast (0));
}
