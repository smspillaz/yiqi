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

    /* The name of this test binary */
    arguments.push_back (argv[0]);

    /* Tool arguments */
    std::string const &toolOption (tool.WrapperOptions ());

    if (!toolOption.empty ())
        arguments.push_back (toolOption);

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
    bool commit = false;
    yu::ExceptionCleanup cleanup ([&] {

        /* Roll back storage of new string */
        auto storedNewStringsIterator =
            std::find (priv->storedNewStrings.begin (),
                       priv->storedNewStrings.end (),
                       value);

        char const *storedStringPtr = nullptr;

        if (storedNewStringsIterator != priv->storedNewStrings.end ())
        {
            storedStringPtr = storedNewStringsIterator->c_str ();
            priv->storedNewStrings.erase (storedNewStringsIterator);
        }

        /* Only if set to a value - we do not
         * want to remove the null-terminator from
         * the vector */
        if (storedStringPtr)
        {
            /* If we have a stored string ptr, try and
             * find that and remove it too */
            auto arrayIterator = std::find (priv->vector.begin (),
                                            priv->vector.end (),
                                            storedStringPtr);

            if (arrayIterator != priv->vector.end ())
                priv->vector.erase (arrayIterator);
        }
    }, commit);

    /* Store a new string so that we can point to it
     * later */
    priv->storedNewStrings.push_back (value);

    /* Should be null-terminator */
    auto rit (priv->vector.rbegin ());

    assert (*rit == NULL);

    /* Go back to the one before it */
    ++rit;

    /* it will point to the null-terminator */
    std::vector <char const *>::iterator it (rit.base ());

    /* insert the new keyEqValue before it */
    priv->vector.insert (it, priv->storedNewStrings.back ().c_str ());

    commit = true;
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
