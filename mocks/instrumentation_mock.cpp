/*
 * instrumentation_mock.cpp:
 * A Google Mock implementation of yiqi::instrumentation::tools::Tool
 *
 * See LICENCE.md for Copyright information
 */

#include "instrumentation_mock.h"

using ::testing::AtLeast;
using ::testing::ReturnRef;

namespace ymock = yiqi::mock;

namespace
{
    std::string const DefaultWrapperName ("");
    std::string const DefaultWrapperOptions ("");
}

ymock::instrumentation::tools::Tool::Tool ()
{
    ON_CALL (*this, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (DefaultWrapperName));
    ON_CALL (*this, WrapperOptions ())
        .WillByDefault (ReturnRef (DefaultWrapperOptions));
}

ymock::instrumentation::tools::Tool::~Tool ()
{
}

void
ymock::instrumentation::tools::Tool::IgnoreCalls ()
{
    EXPECT_CALL (*this, InstrumentationWrapper ()).Times (AtLeast (0));
    EXPECT_CALL (*this, WrapperOptions ()).Times (AtLeast (0));
    EXPECT_CALL (*this, ToolIdentifier ()).Times (AtLeast (0));
}
