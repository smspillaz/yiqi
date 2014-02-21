/*
 * systempaths.cpp:
 * Tests for the path-splitting functions
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include "systempaths.h"

using ::testing::ElementsAreArray;
using ::testing::Matcher;
using ::testing::StrEq;

namespace ysys = yiqi::system;

namespace
{
    std::string const SinglePath ("/mock/path/");
    std::string const MultiplePaths (SinglePath + ":" + SinglePath);
}

TEST (SystemPath, NullReturnsEmptyVector)
{
    std::vector <std::string> paths (ysys::SplitPathString (nullptr));
    EXPECT_EQ (0UL, paths.size ());
}

TEST (SystemPath, SingleStringNoDelimitedReturnsSingleString)
{
    char const *SinglePathCStr (SinglePath.c_str ());
    std::vector <std::string> paths (ysys::SplitPathString (SinglePathCStr));

    Matcher <std::string> singleMatcher[] =
    {
        StrEq (SinglePath)
    };

    EXPECT_THAT (paths,
                 ElementsAreArray (singleMatcher));
}

TEST (SystemPath, DelimitedStringReturnsMultipleStrings)
{
    char const *MultiplePathsCStr (MultiplePaths.c_str ());
    std::vector <std::string> paths (ysys::SplitPathString (MultiplePathsCStr));

    Matcher <std::string> matcher[] =
    {
        StrEq (SinglePath),
        StrEq (SinglePath)
    };

    EXPECT_THAT (paths,
                 ElementsAreArray (matcher));
}
