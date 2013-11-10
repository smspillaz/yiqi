/*
 * scopeguard.cpp
 * Integration tests for the folly::ScopeGuard template
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include "scopeguard.h"

namespace
{
    class GuardVerifier
    {
        public:

            GuardVerifier () = default;

            MOCK_METHOD0 (Cleanup, void ());

            std::function <void ()> CleanupFuncBinding ();

        private:

            GuardVerifier (GuardVerifier const &) = delete;
            GuardVerifier & operator= (GuardVerifier const &) = delete;
    };
}

std::function <void ()>
GuardVerifier::CleanupFuncBinding ()
{
    return std::bind (&GuardVerifier::Cleanup, this);
}

class ScopeGuard :
    public ::testing::Test
{
    protected:

        GuardVerifier verifier;
};

TEST_F (ScopeGuard, NoThrowNoCleanup)
{
    auto cleanup = yiqi::ScopeGuard (verifier.CleanupFuncBinding ());
    cleanup.dismiss ();

    EXPECT_CALL (verifier, Cleanup ()).Times (0);
}

TEST_F (ScopeGuard, CleanupOnThrowIfNotDismiss)
{
    EXPECT_CALL (verifier, Cleanup ()).Times (1);

    try
    {
        auto cleanup = yiqi::ScopeGuard (verifier.CleanupFuncBinding ());
        throw std::exception ();
    }
    catch (...)
    {
    }
}

TEST_F (ScopeGuard, NoCleanupOnThrowIfDismiss)
{
    EXPECT_CALL (verifier, Cleanup ()).Times (0);

    try
    {
        auto cleanup = yiqi::ScopeGuard (verifier.CleanupFuncBinding ());
        cleanup.dismiss ();
        throw std::exception ();
    }
    catch (...)
    {
    }
}
