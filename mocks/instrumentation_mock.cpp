/*
 * instrumentation_mock.cpp:
 * A Google Mock implementation of yiqi::instrumentation::tools::Tool
 *
 * See LICENCE.md for Copyright information
 */

#include "instrumentation_mock.h"

using ::testing::AtLeast;

namespace ymock = yiqi::mock;

ymock::instrumentation::tools::Tool::Tool ()
{
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
