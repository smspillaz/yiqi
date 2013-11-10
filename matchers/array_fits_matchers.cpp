/*
 * array_fits_matchers.cpp:
 * Provide a Google Test Matcher to evaluate arguments in an array
 * of type ArrayType const * for each Matcher <ArrayType> in an
 * std::vector <Matcher <ArrayType> >.
 *
 * The matched array must have at least the same number of elements
 * as as the provided number of matchers. No size checks are done
 * as this would impose a requirement that the array be terminated
 * be null-terminated
 *
 * See LICENCE.md for Copyright information
 */

#include "array_fits_matchers.h"

namespace ymatchutil = yiqi::matchers::util;

std::ostream &
ymatchutil::InsertIntoStreamIfNotEmpty (std::string const &str,
                                        std::ostream      &os)
{
    if (!str.empty ())
        os << ", " << str;

    return os;
}

ymatchutil::StringMatchResultListener::StringMatchResultListener () :
    MatchResultListener (&mStringStream)
{
}

std::string
ymatchutil::StringMatchResultListener::str () const
{
    return mStringStream.str ();
}
