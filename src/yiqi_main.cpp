/*
 * yiqi_main.cpp:
 * Provide the main () function for the tests, inject our environment.
 *
 * See LICENCE.md for Copyright information
 */
#include <gtest/gtest.h>
#include <yiqi/init.h>
#include <yiqi/platform_gtest.h>

int main (int argc, char **argv)
{
    yiqi::gtest::Platform gtestPlatform;

    ::testing::InitGoogleTest (&argc, argv);
    ::yiqi::Init (gtestPlatform, argc, argv);

    return RUN_ALL_TESTS ();
}
