/*
 * commandline.cpp:
 * Definitions of a function to extract a command line
 * from the currently running program argv and also the
 * instrumentation parameters
 *
 * See LICENCE.md for Copyright information
 */

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <cstring>

#include <assert.h>

#include "commandline.h"
#include "exceptioncleanup.h"
#include "instrumentation_tool.h"

namespace ycom = yiqi::commandline;
namespace yit = yiqi::instrumentation::tools;
namespace yu = yiqi::util;

ycom::CommandArguments
ycom::BuildCommandLine (int                 argc,
                        char const * const  *argv,
                        yit::Tool const     &tool)
{
    if (argc < 1)
        throw std::runtime_error ("provided argc must have at "
                                  "least the prorgram name");

    /* In the usual case we will have three args */
    ycom::CommandArguments arguments;
    arguments.reserve (3);

    /* Instrumentation tool */
    std::string const &wrapper (tool.InstrumentationWrapper ());

    if (!wrapper.empty ())
        arguments.push_back (wrapper);

    /* Tool arguments */
    std::string const &toolOption (tool.WrapperOptions ());

    if (!toolOption.empty ())
        arguments.push_back (toolOption);

    /* The name of this test binary */
    arguments.push_back (argv[0]);

    return arguments;
}

ycom::ArgvVector const
ycom::StringVectorToArgv (CommandArguments const &args)
{
    ycom::ArgvVector argv;

    for (auto const &arg : args)
        argv.push_back (arg.c_str ());

    argv.push_back (nullptr);

    return argv;
}

namespace yiqi
{
    namespace commandline
    {
        class NullTermArray::Private
        {
            public:

                Private ();
                explicit Private (char const * const *array);
                Private (Private const &);
                Private & operator= (Private rhs);

                bool operator== (Private const &rhs) const;
                bool operator!= (Private const &lhs) const;

                friend void swap (Private &lhs, Private &rhs);

                std::vector <char const *> vector;
                std::vector <std::string>  storedNewStrings;

            private:

                static size_t arrayLength (char const * const *);
        };

        void swap (NullTermArray::Private &lhs,
                   NullTermArray::Private &rhs);
    }
}

size_t
ycom::NullTermArray::Private::arrayLength (char const * const *array)
{
    size_t size = 0;
    for (; array[size] != NULL; ++size);
    return size + 1;
}

ycom::NullTermArray::Private::Private ()
{
    /* Always start with a null-terminator */
    vector.push_back (NULL);
}

ycom::NullTermArray::Private::Private (char const * const *array) :
    vector (array, array + arrayLength (array))
{
}

ycom::NullTermArray::Private::Private (Private const &priv) :
    vector (priv.vector)
{
}

void
ycom::swap (NullTermArray::Private &lhs,
            NullTermArray::Private &rhs)
{
    using std::swap;

    swap (lhs.vector, rhs.vector);
}

ycom::NullTermArray::Private &
ycom::NullTermArray::Private::operator= (Private rhs)
{
    using ycom::swap;

    swap (*this, rhs);
    return *this;
}

bool
ycom::NullTermArray::Private::operator== (Private const &rhs) const
{
    return vector == rhs.vector;
}

bool
ycom::NullTermArray::Private::operator!= (Private const &rhs) const
{
    return !(*this == rhs);
}

ycom::NullTermArray::NullTermArray () :
    priv (new Private ())
{
}

ycom::NullTermArray::~NullTermArray ()
{
}

ycom::NullTermArray::NullTermArray (char const * const *envp) :
    priv (new Private (envp))
{
}

ycom::NullTermArray::NullTermArray (NullTermArray const &env) :
    priv (new Private (*env.priv))
{
}

void
ycom::swap (NullTermArray &lhs, NullTermArray &rhs)
{
    using std::swap;
    swap (*lhs.priv, *rhs.priv);
}

ycom::NullTermArray &
ycom::NullTermArray::operator= (NullTermArray rhs)
{
    swap (*this, rhs);
    return *this;
}

bool
ycom::NullTermArray::operator== (NullTermArray const &rhs) const
{
    NullTermArray::Private const &pref = *priv;
    NullTermArray::Private const &rhsref = *rhs.priv;
    return pref == rhsref;
}

bool
ycom::NullTermArray::operator!= (NullTermArray const &lhs) const
{
    return !(*this == lhs);
}

void
ycom::NullTermArray::append (std::string const &value)
{
    std::vector <std::string> vec =
    {
        value
    };

    append (vec);
}

void
ycom::NullTermArray::removeAnyMatching (RemoveFunc const &remover)
{
    auto wrapper = [this, &remover](char const *str) -> bool {
        if (str)
            return remover (str);
        else
            return false;
    };

    priv->vector.erase (std::remove_if (priv->vector.begin (),
                                        priv->vector.end (),
                                        wrapper),
                        priv->vector.end ());
}

void
ycom::NullTermArray::eraseAppended (StringVector const &values)
{
    /* Start search from at least values.size () from the end */
    auto searchStartPoint = priv->storedNewStrings.end () -
                            (values.size ());
    auto firstStoredNewStringsIterator =
        std::find (searchStartPoint,
                   priv->storedNewStrings.end (),
                   values.front ());

    if (firstStoredNewStringsIterator != priv->storedNewStrings.end ())
    {
        auto lastStoredNewStringIterator =
            firstStoredNewStringsIterator;

        bool foundLastString = false;

        /* Keep going until either the end, or until the block
         * where we've inserted stops. We already have the first
         * iterator, so move directly to the second */
        for (auto it = (values.begin () + 1);
             it != values.end ();
             ++it)
        {
            ++lastStoredNewStringIterator;

            if (lastStoredNewStringIterator ==
                    priv->storedNewStrings.end ())
                break;

            if (*lastStoredNewStringIterator != *it)
            {
                foundLastString = true;
                break;
            }
        }

        /* We need to put the last string iterator at
         * one past the position we intend to erase
         * inclusive if it didn't happen already */
        if (!foundLastString)
            ++lastStoredNewStringIterator;

        auto findPointerInVectorFunc =
            [&firstStoredNewStringsIterator](char const *str) -> bool {
                /* We want to compare the pointers, as it was pointers
                 * that were inserted, not new values */
                return firstStoredNewStringsIterator->c_str () == str;
            };

        /* Handle the duplicate-pointers edge case by
         * starting from end - distance (first, last) - 1 */
        auto searchStartPoint =
            priv->vector.end () - std::distance (firstStoredNewStringsIterator,
                                                 lastStoredNewStringIterator) - 1;
        auto firstPointerInVector =
            std::find_if (searchStartPoint,
                          priv->vector.end (),
                          findPointerInVectorFunc);

        if (firstPointerInVector != priv->vector.end ())
        {
            bool foundLastPointer = false;

            /* Keep going until we find the end of strings
             * that were stored here. We already have the first
             * iterator, so move directly to the second */
            auto lastPointerInVector = firstPointerInVector;

            for (auto it = (values.begin () + 1);
                 it != values.end ();
                 ++it)
            {
                ++lastPointerInVector;

                if (lastPointerInVector == priv->vector.end ())
                    break;

                if (*lastPointerInVector != it->c_str ())
                {
                    foundLastPointer = true;
                    break;
                }
            }

            /* We need to put the last string iterator at
             * one past the position we intend to erase
             * inclusive if it didn't happen already */
            if (!foundLastPointer)
                ++lastPointerInVector;

            /* Erase this from the vector of pointers */
            priv->vector.erase (firstPointerInVector,
                                lastPointerInVector);
        }

        /* Erase this block from the vector of stored
         * new strings */
        priv->storedNewStrings.erase (firstStoredNewStringsIterator,
                                      lastStoredNewStringIterator);
    }
}

void
ycom::NullTermArray::append (std::vector <std::string> const &vec)
{
    auto cleanupFunc = std::bind (&ycom::NullTermArray::eraseAppended,
                                  this,
                                  vec);
    yu::ExceptionCleanup cleanup (cleanupFunc);

    /* Reserve some more space */
    size_t oldLength = priv->storedNewStrings.size ();

    priv->storedNewStrings.insert (priv->storedNewStrings.end (),
                                   vec.begin (),
                                   vec.end ());

    /* Go to first newly stored string */
    auto firstNewlyStoredString (priv->storedNewStrings.begin ());
    std::advance (firstNewlyStoredString, oldLength);

    for (auto it = firstNewlyStoredString;
         it != priv->storedNewStrings.end ();
         ++it)
    {
        /* Should be null-terminator */
        auto nullTerminatorPosition (priv->vector.end () - 1);

        assert (*nullTerminatorPosition == NULL);

        /* Insert the next pointer before the null terminator */
        priv->vector.insert (nullTerminatorPosition,
                             it->c_str ());
    }

    cleanup.commit ();
}

void
ycom::InsertEnvironmentPair (NullTermArray &array,
                             char const    *key,
                             char const    *value)
{
    std::string const KeyEqValue (std::string (key) +
                                  "=" +
                                  std::string (value));
    array.append (KeyEqValue);
}

char const * const *
ycom::NullTermArray::underlyingArray () const
{
    return &(priv->vector[0]);
}

size_t
ycom::NullTermArray::underlyingArrayLen () const
{
    return priv->vector.size ();
}
