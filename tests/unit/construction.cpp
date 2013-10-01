/*
 * construction.h:
 * Provide header declarations for functions to determine
 * how the test environment should be constructed
 *
 * See LICENCE.md for Copyright information
 */

#include <vector>

#include <gmock/gmock.h>

#include <construction.h>
#include <constants.h>
#include <instrumentation_tool.h>

#include "test_util.h"

using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Matcher;
using ::testing::NotNull;
using ::testing::StrEq;
using ::testing::WithParamInterface;
using ::testing::ValuesIn;

namespace ytest = yiqi::testing;
namespace yconst = yiqi::constants;
namespace yc = yiqi::construction;
namespace yit = yiqi::instrumentation::tools;
namespace po = boost::program_options;

namespace
{
    typedef ytest::CommandLineArguments CommandLineArguments;
    int ArgumentCount (ytest::CommandLineArguments const &args)
    {
        return ytest::ArgumentCount (args);
    }

    char const * const * Arguments (CommandLineArguments const &args)
    {
        return ytest::Arguments (args);
    }

    /* Constants */
    static std::string const MockTool ("mock");
    static std::string const ArgYiqiToolOption (std::string ("--") +
                                                yconst::YiqiToolOption);

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
    return ytest::GenerateCommandLine (arguments);
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
    MatchAnythingFor (ytest::MockProgramName, matchers);

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
    MatchExact (ytest::MockProgramName, matchers);

    for (std::string const &str : SampleCommandArguments)
        MatchAnythingFor (str, matchers);

    EXPECT_THAT (ArgumentsToVector (args),
                 ElementsAreArray (&matchers[0],
                                   matchers.size ()));
}

TEST_F (ConstructionParameters, ParseOptionsToParametersReturnsNonNullPointer)
{
    CommandLineArguments args (GenerateCommandLine (RealCommandArguments));

    auto pointer (yc::ParseOptionsToToolUniquePtr (ArgumentCount (args),
                                                   Arguments (args),
                                                   desc));

    EXPECT_THAT (pointer.get (), NotNull ());
}

TEST_F (ConstructionParameters, ParseOptionsForToolReturnsSpecifiedTool)
{
    CommandLineArguments args (GenerateCommandLine (RealCommandArguments));

    std::string tool (yc::ParseOptionsForToolName (ArgumentCount (args),
                                                   Arguments (args),
                                                   desc));
    EXPECT_EQ (MockTool, tool);
}

TEST_F (ConstructionParameters, ParseOptionsForToolReturnsNoneIfNoOption)
{
    CommandLineArguments args (GenerateCommandLine (NoArguments));

    auto const toolId (yconst::InstrumentationTool::None);
    std::string ExpectedTool (yconst::StringFromTool (toolId));

    std::string tool (yc::ParseOptionsForToolName (ArgumentCount (args),
                                                   Arguments (args),
                                                   desc));

    EXPECT_EQ (ExpectedTool, tool);
}

class ConstructionParametersTable :
    public ConstructionParameters,
    public ::testing::WithParamInterface <yconst::InstrumentationToolName>
{
};

TEST_P (ConstructionParametersTable, MakeSpecifiedToolReturnsExpectedTool)
{
    yconst::InstrumentationTool const ExpectedIdentifier (GetParam ().tool);
    yit::Program::Unique tool (yc::MakeProgramInfo (ExpectedIdentifier));

    EXPECT_EQ (ExpectedIdentifier, tool->ToolIdentifier ());
}

TEST_P (ConstructionParametersTable, ParseOptionsForToolReturnsExpectedTool)
{
    yconst::InstrumentationTool const ExpectedIdentifier (GetParam ().tool);
    std::vector <std::string> const ArgumentsVector =
    {
        ArgYiqiToolOption,
        std::string (GetParam ().name)
    };

    CommandLineArguments args (GenerateCommandLine (ArgumentsVector));

    auto tool (yc::ParseOptionsToToolUniquePtr (ArgumentCount (args),
                                                Arguments (args),
                                                desc));
    EXPECT_EQ (ExpectedIdentifier, tool->ToolIdentifier ());
}

INSTANTIATE_TEST_CASE_P (AvailableTools, ConstructionParametersTable,
                         ValuesIn (yconst::InstrumentationToolNames ()));
