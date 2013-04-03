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
#include <constants.h>

#include "test_util.h"

using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Matcher;
using ::testing::NotNull;
using ::testing::StrEq;

namespace ytest = yiqi::testing;
namespace yconst = yiqi::constants;
namespace yc = yiqi::construction;
namespace po = boost::program_options;

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

    /* Constants */
    static std::string const MockTool ("mock");
    static std::string const MockProgramName ("mock_program_name");
    static std::string const ArgYiqiToolOption (std::string ("--") +
                                                yconst::YiqiToolOption ());

    std::vector <std::string> const SampleCommandArguments =
    {
        ArgYiqiToolOption,
        "--yiqi_a",
        "--yiqi_b"
    };

    const std::vector <std::string> RealCommandArguments =
    {
        ArgYiqiToolOption,
        MockTool
    };

    const std::vector <std::string> NoArguments;
}

class ConstructionParameters :
    public ::testing::Test
{
    public:

        ConstructionParameters () :
            desc (yc::FetchOptionsDescription ())
        {
        }

        CommandLineArguments
        GenerateCommandLine (std::vector <std::string> const &);

    protected:

        po::options_description desc;
};

CommandLineArguments
ConstructionParameters::GenerateCommandLine (std::vector<std::string> const &arguments)
{
    std::vector <const char *> charArguments;

    charArguments.push_back (MockProgramName.c_str ());

    for (std::string const &str : arguments)
        charArguments.push_back (str.c_str ());

    return CommandLineArguments (charArguments.size (),
                                 charArguments);
}

TEST_F (ConstructionParameters, GeneratedCommandLineHasNPlusOneProvidedArguments)
{
    CommandLineArguments args (GenerateCommandLine (SampleCommandArguments));

    EXPECT_EQ (SampleCommandArguments.size () + 1, ArgumentCount (args));
}

namespace
{
    void MatchAnythingFor (std::string const                     &str,
                           std::vector <Matcher <char const *> > &matchers)
    {
        matchers.push_back (_);
    }

    void MatchExact (std::string const                     &str,
                     std::vector <Matcher <char const *> > &matchers)
    {
        matchers.push_back (StrEq (str));
    }

    std::vector <char const *> ArgumentsToVector (CommandLineArguments const &args)
    {
        return ytest::ToVector (Arguments (args),
                                ArgumentCount (args));
    }
}

TEST_F (ConstructionParameters, GeneratedCommandLineHasAllArguments)
{
    CommandLineArguments args (GenerateCommandLine (SampleCommandArguments));

    std::vector <Matcher <char const *> > matchers;
    MatchAnythingFor (MockProgramName, matchers);

    for (std::string const &str : SampleCommandArguments)
        MatchExact (str, matchers);

    /* ElementsAreArray has some strange semantics:
     *
     * 1. The matched argument must be a container, it cannot
     *    be an array
     * 2. You can't pass an STL container of matchers to ElementsAreArray,
     *    you can only pass an array, which means that it needs to know
     *    the size of the array too
     */
    EXPECT_THAT (ArgumentsToVector (args),
                 ElementsAreArray (&matchers[0],
                                   matchers.size ()));
}

TEST_F (ConstructionParameters, GeneratedCommandLineHasFirstArgAsMockProgramName)
{
    CommandLineArguments args (GenerateCommandLine (SampleCommandArguments));

    std::vector <Matcher <char const *> > matchers;
    MatchExact (MockProgramName, matchers);

    for (std::string const &str : SampleCommandArguments)
        MatchAnythingFor (str, matchers);

    EXPECT_THAT (ArgumentsToVector (args),
                 ElementsAreArray (&matchers[0],
                                   matchers.size ()));
}

TEST_F (ConstructionParameters, ParseOptionsToParametersReturnsNonNullPointer)
{
    CommandLineArguments args (GenerateCommandLine (RealCommandArguments));

    auto pointer (yc::ParseOptionsToParameters (ArgumentCount (args),
                                                Arguments (args),
                                                desc));

    EXPECT_THAT (pointer.get (), NotNull ());
}

TEST_F (ConstructionParameters, ParseOptionsForToolReturnsSpecifiedTool)
{
    CommandLineArguments args (GenerateCommandLine (RealCommandArguments));

    std::string tool (yc::ParseOptionsForTool (ArgumentCount (args),
                                               Arguments (args),
                                               desc));
    EXPECT_EQ (MockTool, tool);
}

TEST_F (ConstructionParameters, ParseOptionsForToolReturnsNoneIfNoOption)
{
    CommandLineArguments args (GenerateCommandLine (NoArguments));

    std::string tool (yc::ParseOptionsForTool (ArgumentCount (args),
                                               Arguments (args),
                                               desc));
    EXPECT_EQ (std::string ("none"), tool);
}
