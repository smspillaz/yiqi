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

    return argv;
}

namespace yiqi
{
    namespace commandline
    {
        class Environment::Private
        {
            public:

                Private ();
                explicit Private (char const * const *envp);
                Private (Private const &);
                Private & operator= (Private rhs);

                bool operator== (Private const &rhs) const;
                bool operator!= (Private const &lhs) const;

                friend void swap (Private &lhs, Private &rhs);

                std::vector <char const *> environment;
                std::vector <std::string>  storedNewStrings;

            private:

                static size_t arrayLength (char const * const *);
        };

        void swap (Environment::Private &lhs,
                   Environment::Private &rhs);
    }
}

size_t
ycom::Environment::Private::arrayLength (char const * const *envp)
{
    size_t size = 0;
    for (; envp[size] != NULL; ++size);
    return size + 1;
}

ycom::Environment::Private::Private ()
{
    /* Always start with a null-terminator */
    environment.push_back (NULL);
}

ycom::Environment::Private::Private (char const * const *envp) :
    environment (envp, envp + arrayLength (envp))
{
}

ycom::Environment::Private::Private (Private const &priv) :
    environment (priv.environment)
{
}

void
ycom::swap (Environment::Private &lhs,
            Environment::Private &rhs)
{
    using std::swap;

    swap (lhs.environment, rhs.environment);
}

ycom::Environment::Private &
ycom::Environment::Private::operator= (Private rhs)
{
    using ycom::swap;

    swap (*this, rhs);
    return *this;
}

bool
ycom::Environment::Private::operator== (Private const &rhs) const
{
    return environment == rhs.environment;
}

bool
ycom::Environment::Private::operator!= (Private const &rhs) const
{
    return !(*this == rhs);
}

ycom::Environment::Environment () :
    priv (new Private ())
{
}

ycom::Environment::~Environment ()
{
}

ycom::Environment::Environment (char const * const *envp) :
    priv (new Private (envp))
{
}

ycom::Environment::Environment (Environment const &env) :
    priv (new Private (*env.priv))
{
}

void
ycom::swap (Environment &lhs, Environment &rhs)
{
    using std::swap;
    swap (*lhs.priv, *rhs.priv);
}

ycom::Environment &
ycom::Environment::operator= (Environment rhs)
{
    swap (*this, rhs);
    return *this;
}

bool
ycom::Environment::operator== (Environment const &rhs) const
{
    Environment::Private const &pref = *priv;
    Environment::Private const &rhsref = *rhs.priv;
    return pref == rhsref;
}

bool
ycom::Environment::operator!= (Environment const &lhs) const
{
    return !(*this == lhs);
}

void
ycom::Environment::insert (char const *variable,
                           char const *value)
{
    std::string const KeyEqValue (std::string (variable) +
                                  "=" +
                                  std::string (value));
    bool commit = false;
    yu::ExceptionCleanup cleanup ([&] {

        /* Roll back storage of new string */
        auto storedNewStringsIterator =
            std::find (priv->storedNewStrings.begin (),
                       priv->storedNewStrings.end (),
                       KeyEqValue);

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
            auto environmentIterator = std::find (priv->environment.begin (),
                                                  priv->environment.end (),
                                                  storedStringPtr);

            if (environmentIterator != priv->environment.end ())
                priv->environment.erase (environmentIterator);
        }
    }, commit);

    /* Store a new string so that we can point to it
     * later */
    priv->storedNewStrings.push_back (KeyEqValue);

    /* Should be null-terminator */
    auto rit (priv->environment.rbegin ());

    assert (*rit == NULL);

    /* Go back to the one before it */
    ++rit;

    /* it will point to the null-terminator */
    std::vector <char const *>::iterator it (rit.base ());

    /* insert the new keyEqValue before it */
    priv->environment.insert (it, priv->storedNewStrings.back ().c_str ());

    commit = true;
}

char const * const *
ycom::Environment::underlyingEnvironmentArray () const
{
    return &(priv->environment[0]);
}

size_t
ycom::Environment::underlyingEnvironmentArrayLen () const
{
    return priv->environment.size ();
}
