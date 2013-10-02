/*
 * instrumentation_mock.cpp:
 * A Google Mock implementation of yiqi::instrumentation::tools::Tool
 *
 * See LICENCE.md for Copyright information
 */

#include "instrumentation_mock.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::ReturnRef;

namespace ymock = yiqi::mock;

namespace
{
    std::string const DefaultWrapperName ("");
    std::string const DefaultWrapperOptions ("");
    std::string const DefaultName ("");
}

ymock::instrumentation::tools::Program::Program ()
{
    ON_CALL (*this, WrapperBinary ())
        .WillByDefault (ReturnRef (DefaultWrapperName));
    ON_CALL (*this, WrapperOptions ())
        .WillByDefault (ReturnRef (DefaultWrapperOptions));
    ON_CALL (*this, Name ())
        .WillByDefault (ReturnRef (DefaultName));

}

ymock::instrumentation::tools::Program::~Program ()
{
}

void
ymock::instrumentation::tools::Program::IgnoreCalls ()
{
    EXPECT_CALL (*this, WrapperBinary ()).Times (AtLeast (0));
    EXPECT_CALL (*this, Name ()).Times (AtLeast (0));
    EXPECT_CALL (*this, WrapperOptions ()).Times (AtLeast (0));
    EXPECT_CALL (*this, ToolIdentifier ()).Times (AtLeast (0));
}

ymock::instrumentation::tools::Controller::Controller ()
{
}

ymock::instrumentation::tools::Controller::~Controller ()
{
}

void
ymock::instrumentation::tools::Controller::IgnoreCalls ()
{
    EXPECT_CALL (*this, ToolIdentifier ()).Times (AtLeast (0));
    EXPECT_CALL (*this, Start ()).Times (AtLeast (0));
    EXPECT_CALL (*this, Stop (_)).Times (AtLeast (0));
}
