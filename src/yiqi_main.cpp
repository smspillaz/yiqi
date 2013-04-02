/*
 * yiqi_main.cpp:
 * Provide the main () function for the tests, parse options,
 * inject environment and re-exec () as necessary.
 *
 * See LICENCE.md for Copyright information
 */

#include <gtest/gtest.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace
{
    class YiqiEnvironment :
        public ::testing::Environment
    {
        public:

            virtual ~YiqiEnvironment () = default;
            virtual void SetUp ();
            virtual void TearDown ();
    };
}

void
YiqiEnvironment::SetUp ()
{
}

void
YiqiEnvironment::TearDown()
{
}

int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    ::testing::AddGlobalTestEnvironment(new YiqiEnvironment);

    return RUN_ALL_TESTS ();
}
