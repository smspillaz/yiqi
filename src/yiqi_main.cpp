/*
 * yiqi_main.cpp:
 * Provide the main () function for the tests, inject our environment.
 *
 * See LICENCE.md for Copyright information
 */
#include <gtest/gtest.h>
#include <yiqi/environment.h>

int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    ::testing::AddGlobalTestEnvironment(yiqi::CreateTestEnvironment (argc,
                                                                     argv));

    return RUN_ALL_TESTS ();
}
