/*
 * platform_gtest.cpp:
 *
 * Platform implementation for the Google Test library.
 *
 * See LICENCE.md for Copyright information
 */

#include <gtest/gtest.h>

#include <yiqi/platform_gtest.h>

#include "instrumentation_tool.h"

using ::testing::EmptyTestEventListener;
using ::testing::TestEventListener;
using ::testing::TestInfo;
using ::testing::UnitTest;

namespace ygt = yiqi::gtest;
namespace yit = yiqi::instrumentation::tools;

namespace
{
    class YiqiGTestListener :
        public EmptyTestEventListener
    {
        public:

            YiqiGTestListener (std::unique_ptr <yit::Controller> &&ctrl);

        private:

            std::unique_ptr <yit::Controller> controller;

            void OnTestStart (TestInfo const &test) override;

            void OnTestIterationStart (UnitTest const &test,
                                       int            iteration) override;

            void OnTestIterationEnd (UnitTest const &test,
                                     int            iteration) override;

            void OnTestEnd (TestInfo const &test) override;

            ygt::Controller::Status currentResults;
    };
}

YiqiGTestListener::YiqiGTestListener (std::unique_ptr <yit::Controller> &&ctl) :
    controller (std::move (ctl))
{
}

// cppcheck-suppress unusedFunction
void YiqiGTestListener::OnTestStart (TestInfo const &test)
{
}

// cppcheck-suppress unusedFunction
void YiqiGTestListener::OnTestEnd (TestInfo const &test)
{
}

// cppcheck-suppress unusedFunction
void YiqiGTestListener::OnTestIterationStart (UnitTest const &test,
                                              int            iteration)
{
    controller->Start ();
}

// cppcheck-suppress unusedFunction
void YiqiGTestListener::OnTestIterationEnd (UnitTest const &test,
                                            int            iteration)
{
    currentResults = controller->Stop ();
}

void ygt::Platform::Register (std::unique_ptr <Controller> &&ctl)
{
    auto &listeners (UnitTest::GetInstance ()->listeners ());
    listeners.Append (new YiqiGTestListener (std::move (ctl)));
}
