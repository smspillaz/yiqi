/*
 * yiqi_tests_main.cpp:
 * Provide the main () function for our tests, set the appropriate reporter.
 *
 * See LICENCE.md for Copyright information
 */
#include <gtest/gtest.h>

#include <cstdlib>

#include <tap.h>

int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);

    if (::getenv ("TAP_OUTPUT"))
    {
        auto &listeners (::testing::UnitTest::GetInstance ()->listeners ());

        delete listeners.Release (listeners.default_result_printer ());
        listeners.Append (new tap::TapListener ());
    }

    return RUN_ALL_TESTS ();
}
