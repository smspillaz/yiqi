/*
 * construction.h:
 * Provide header declarations for functions to determine
 * how the test environment should be constructed
 *
 * See LICENCE.md for Copyright information
 */

#include <tuple>
#include <vector>
#include <functional>

#include <gmock/gmock.h>

#include <construction.h>

using ::testing::ElementsAreArray;
using ::testing::Matcher;
using ::testing::StrEq;

namespace
{
    /**
     * @brief The ExceptionCleanup class provides a method to guaruntee
     * cleanup of a function if an exception is thrown before commit is
     * set to true
     */
    class ExceptionCleanup
    {
	public:

	    typedef std::function <void ()> Func;
	    ExceptionCleanup (Func const &func,
			      bool       &commit) :
		mFunc (func),
		mCommit (commit)
	    {
	    }

	    ~ExceptionCleanup ()
	    {
		if (!mCommit)
		    mFunc ();
	    }

	private:

	    ExceptionCleanup (ExceptionCleanup const &) = delete;
	    ExceptionCleanup & operator= (ExceptionCleanup const &) = delete;

	    Func mFunc;
	    bool &mCommit;
    };

    /**
     * @brief A tuple type with a managed array of arguments
     */
    typedef std::tuple <int, std::vector <char const *> > CommandLineArguments;

    int ArgumentCount (CommandLineArguments const &args)
    {
	return std::get <0> (args);
    }

    char const * const * Arguments (CommandLineArguments const &args)
    {
	return &(std::get <1> (args))[0];
    }

    /**
     * @brief Copy array into vector so we can use it with Google Test matchers
     */
    template <typename T>
    std::vector <T> ToVector (T const *array, size_t size)
    {
	if (size)
	    return std::vector <T> (array, array + size);
	else
	    throw std::out_of_range ("ToVector: size is < 1");
    }

    /* Constants */
    const std::vector <std::string> SampleCommandArguments =
    {
	"--yiqi_tool",
	"--yiqi_a",
	"--yiqi_b"
    };
}

class ConstructionParameters :
    public ::testing::Test
{
    public:

	CommandLineArguments
	GenerateCommandLine (std::vector <std::string> const &);
};

CommandLineArguments
ConstructionParameters::GenerateCommandLine (std::vector<std::string> const &arguments)
{
    std::vector <const char *> charArguments;

    for (std::string const &str : arguments)
	charArguments.push_back (str.c_str ());

    return CommandLineArguments (charArguments.size (),
				 charArguments);
}

TEST_F (ConstructionParameters, GeneratedCommandLineHasNProvidedArguments)
{
    CommandLineArguments args (GenerateCommandLine (SampleCommandArguments));

    EXPECT_EQ (SampleCommandArguments.size (), ArgumentCount (args));
}

TEST_F (ConstructionParameters, GeneratedCommandLineHasAllArguments)
{
    CommandLineArguments args (GenerateCommandLine (SampleCommandArguments));

    std::vector <Matcher <char const *> > matchers;
    for (std::string const &str : SampleCommandArguments)
	matchers.push_back (StrEq (str));

    /* ElementsAreArray has some strange semantics:
     *
     * 1. The matched argument must be a container, it cannot
     *    be an array
     * 2. You can't pass an STL container of matchers to ElementsAreArray,
     *    you can only pass an array, which means that it needs to know
     *    the size of the array too
     */
    EXPECT_THAT (ToVector (Arguments (args),
			   ArgumentCount (args)),
		 ElementsAreArray (&matchers[0],
				   matchers.size ()));
}
