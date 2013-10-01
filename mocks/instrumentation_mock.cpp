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
    std::string const DefaultInstrumentationName ("");
}

ymock::instrumentation::tools::Program::Program ()
{
    ON_CALL (*this, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (DefaultWrapperName));
    ON_CALL (*this, WrapperOptions ())
        .WillByDefault (ReturnRef (DefaultWrapperOptions));
    ON_CALL (*this, InstrumentationName ())
        .WillByDefault (ReturnRef (DefaultInstrumentationName));

}

ymock::instrumentation::tools::Program::~Program ()
{
}

void
ymock::instrumentation::tools::Program::IgnoreCalls ()
{
    EXPECT_CALL (*this, InstrumentationWrapper ()).Times (AtLeast (0));
    EXPECT_CALL (*this, InstrumentationName ()).Times (AtLeast (0));
    EXPECT_CALL (*this, WrapperOptions ()).Times (AtLeast (0));
    EXPECT_CALL (*this, ToolIdentifier ()).Times (AtLeast (0));
}
