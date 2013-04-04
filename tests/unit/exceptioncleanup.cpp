/*
 * exceptioncleanup.cpp
 * Unit tests for the yiqi::util::ExceptionCleanup
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include <exceptioncleanup.h>

namespace yu = yiqi::util;

namespace
{
    class CleanupVerifier
    {
        public:

            CleanupVerifier () = default;

            MOCK_METHOD0 (Cleanup, void ());

            yu::ExceptionCleanup::Func CleanupFuncBinding ();

        private:

            CleanupVerifier (CleanupVerifier const &) = delete;
            CleanupVerifier & operator= (CleanupVerifier const &) = delete;
    };
}

yu::ExceptionCleanup::Func
CleanupVerifier::CleanupFuncBinding ()
{
    return std::bind (&CleanupVerifier::Cleanup, this);
}

class ExceptionCleanup :
    public ::testing::Test
{
    protected:

        CleanupVerifier verifier;
};

TEST_F (ExceptionCleanup, NoThrowNoCleanup)
{
    bool commit = true;
    yu::ExceptionCleanup cleanup (verifier.CleanupFuncBinding (), commit);

    EXPECT_CALL (verifier, Cleanup ()).Times (0);
}

TEST_F (ExceptionCleanup, CleanupOnThrowIfNotCommit)
{
    bool commit = false;

    EXPECT_CALL (verifier, Cleanup ()).Times (1);

    try
    {
        yu::ExceptionCleanup cleanup (verifier.CleanupFuncBinding (), commit);
        std::cout << "before end .." << std::endl;
    }
    catch (...)
    {
    }
}

TEST_F (ExceptionCleanup, NoCleanupOnThrowIfCommit)
{
    bool commit = false;

    EXPECT_CALL (verifier, Cleanup ()).Times (0);

    try
    {
        yu::ExceptionCleanup cleanup (verifier.CleanupFuncBinding (), commit);
        commit = true;
        throw std::exception ();
    }
    catch (...)
    {
    }
}


