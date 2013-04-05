/*
 * array_fits_matchers.h:
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

#ifndef YIQI_ARRAY_FITS_MATCHERS_H
#define YIQI_ARRAY_FITS_MATCHERS_H

#include <iostream>
#include <vector>
#include <gmock/gmock.h>

namespace yiqi
{
    namespace matchers
    {
        template <typename ArrayType>
        class ArrayMatcher :
            public ::testing::MatcherInterface <ArrayType const *>
        {
            public:

                typedef ::testing::Matcher <ArrayType> ChildMatcher;
                typedef std::vector <ChildMatcher> ChildMatcherVector;
                ArrayMatcher (ChildMatcherVector const &matchers) :
                    mMatchers (matchers)
                {
                }

                void DescribeTo (std::ostream *os) const
                {
                    size_t n (count ());

                    if (n == 0)
                    {
                        *os << "matching no elements";
                        return;
                    }
                    else if (n == 1)
                    {
                        *os << "matches the first element which is ";
                        mMatchers[0].DescribeTo (os);
                    }
                    else
                    {
                        *os << "matches the first "
                            << n << " elements which are"
                            << std::endl;

                        for (size_t i = 0; i < n; ++i)
                        {
                            *os << "element #" << i << " ";
                            mMatchers[i].DescribeTo (os);
                            *os << "," << std::endl;
                        }
                    }
                }

                void DescribeNegationTo (std::ostream *os) const
                {
                    size_t n (count ());

                    ASSERT_TRUE (n > 0) << "matching zero elements must "
                                           "always pass";

                    if (n == 1)
                    {
                        *os << "doesn't match the first element which is ";
                        mMatchers[0].DescribeTo (os);
                    }
                    else
                    {
                        *os << "doesn't match the first "
                            << n << " elements which are"
                            << std::endl;

                        for (size_t i = 0; i < n; ++i)
                        {
                            *os << "element #" << i << " ";
                            mMatchers[i].DescribeTo (os);
                            *os << "," << std::endl;
                        }
                    }
                }

                typedef ::testing::MatchResultListener MRL;

                bool MatchAndExplain (ArrayType const *arr,
                                      MRL             *listener) const
                {
                    size_t n (count ());

                    /* We can pass instantly if we are not matching any
                     * of the elements */
                    if (n == 0)
                        return true;
                    else
                    {
                        if (arr)
                        {
                            for (size_t i = 0; i < n; ++i)
                            {
                                namespace ti = ::testing::internal;
                                ti::StringMatchResultListener s;
                                bool success =
                                    mMatchers[i].MatchAndExplain (arr[i], &s);

                                if (!success)
                                {
                                    std::ostream *stream (listener->stream ());
                                    *listener << "whose element #" << i
                                              << "doesn't match";
                                    ti::PrintIfNotEmpty (s.str (),
                                                         stream);
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            std::ostream *os (listener->stream ());
                            if (os)
                            {
                                std::string msg ("tries to match elements "
                                                 "in an empty array");
                                *os << msg << std::endl;
                            }

                            return false;
                        }
                    }

                    return true;
                }

            private:

                size_t count () const
                {
                    return mMatchers.size ();
                }

                std::vector < ::testing::Matcher <ArrayType> > const mMatchers;
        };

        namespace t = ::testing;

        template <typename T>
        ::testing::Matcher <T const *>
        ArrayFitsMatchers (std::vector <t::Matcher <T> > const &matchers)
        {
            return ::testing::MakeMatcher (new ArrayMatcher <T> (matchers));
        }
    }
}

#endif // YIQI_ARRAY_FITS_MATCHERS_H
