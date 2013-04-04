/*
 * commandline.cpp:
 * Tests for building the command line
 *
 * See LICENCE.md for Copyright information
 */

#include <gmock/gmock.h>

#include "commandline.h"
#include "instrumentation_tool.h"
#include "test_util.h"
#include "value_type_test.h"

#include "instrumentation_mock.h"

using ::testing::AtLeast;
using ::testing::ElementsAreArray;
using ::testing::IsNull;
using ::testing::Matcher;
using ::testing::ReturnRef;
using ::testing::StrEq;

namespace ycom = yiqi::commandline;
namespace yit = yiqi::instrumentation::tools;
namespace ymock = yiqi::mock;
namespace ymockit = yiqi::mock::instrumentation::tools;
namespace ytest = yiqi::testing;

namespace
{
    static std::string const MockWrapper ("mock");
    static std::string const MockOptions ("--mock");
    static std::string const NilString ("");
    static std::vector <std::string> const NoOptions;

    typedef ytest::CommandLineArguments CommandLineArguments;

    CommandLineArguments
    GenerateCommandLine (std::vector <std::string> const &args)
    {
        return ytest::GenerateCommandLine (args);
    }
}

class BuildCommandLine :
    public ::testing::Test
{
    public:

        BuildCommandLine () :
            instrumentation (new ymockit::Tool)
        {
            /* We don't care about whether or not these functions are
             * called, we care about how the system under test handles
             * them */
            EXPECT_CALL (*instrumentation, InstrumentationWrapper ())
                .Times (AtLeast (0));
            EXPECT_CALL (*instrumentation, WrapperOptions ())
                .Times (AtLeast (0));
        }

    protected:

        std::unique_ptr <ymockit::Tool> instrumentation;
};

TEST_F (BuildCommandLine, ThrowOnNoArgs)
{
    CommandLineArguments originalArgs;
    EXPECT_THROW ({
        ycom::BuildCommandLine (ytest::ArgumentCount (originalArgs),
                                ytest::Arguments (originalArgs),
                                *instrumentation);
    },
    std::runtime_error);
}

TEST_F (BuildCommandLine, NoWrapperOrOptionsJustArgvReturnsArgvAt0)
{
    CommandLineArguments originalArgs (GenerateCommandLine (NoOptions));

    ON_CALL (*instrumentation, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (NilString));
    ON_CALL (*instrumentation, WrapperOptions ())
        .WillByDefault (ReturnRef (NilString));

    auto args (ycom::BuildCommandLine (ytest::ArgumentCount (originalArgs),
                                       ytest::Arguments (originalArgs),
                                       *instrumentation));

    Matcher <std::string> matchers[] = { StrEq (ytest::MockProgramName ()) };

    EXPECT_THAT (args,
                 ElementsAreArray (matchers));
}

TEST_F (BuildCommandLine, ReturnWrapperAndOptions)
{
    CommandLineArguments originalArgs (GenerateCommandLine (NoOptions));

    ON_CALL (*instrumentation, InstrumentationWrapper ())
        .WillByDefault (ReturnRef (MockWrapper));
    ON_CALL (*instrumentation, WrapperOptions ())
        .WillByDefault (ReturnRef (MockOptions));

    auto args (ycom::BuildCommandLine (ytest::ArgumentCount (originalArgs),
                                       ytest::Arguments (originalArgs),
                                       *instrumentation));

    Matcher <std::string> matchers[] =
    {
        StrEq (MockWrapper),
        StrEq (ytest::MockProgramName ()),
        StrEq (MockOptions)
    };

    EXPECT_THAT (args,
                 ElementsAreArray (matchers));
}

TEST (CommandLine, VectorToArgv)
{
    std::string const MockItem ("item");
    std::string const MockItem2 ("item2");

    ycom::CommandArguments const args =
    {
        MockItem,
        MockItem2
    };

    Matcher <char const *> matchers[] =
    {
        StrEq (MockItem),
        StrEq (MockItem2),
    };

    ycom::ArgvVector const argv (ycom::StringVectorToArgv (args));

    EXPECT_THAT (argv,
                 ElementsAreArray (matchers));
}

namespace
{
    std::string const MockEnvKey ("MOCK");
    std::string const MockerEnvKey ("MOCKER");
    std::string const MockEnvValue ("mock");
    std::string const MockEnvKeyValue (MockEnvKey + "=" + MockEnvValue);

    static char const * const NonDefaultMockEnvp[] =
    {
        MockEnvKeyValue.c_str (),
        NULL
    };
}

namespace yiqi
{
    namespace testing
    {
        template <>
        class ValueTypeConstructor <ycom::Environment>
        {
            public:

                ycom::Environment ConstructWithNonDefaultData ();
                bool HasNonDefaultConstructor ();
                bool Mutate (ycom::Environment &env);
                bool Overloaded ();
        };
    }

    namespace commandline
    {
        std::ostream & operator<< (std::ostream &os, Environment const &env);
    }
}

std::ostream &
ycom::operator<< (std::ostream &os, Environment const &env)
{
    std::string const nullTerminator ("null-terminator");
    auto envVector (ytest::ToVector <char const *> (
                        env.underlyingEnvironmentArray (),
                        env.underlyingEnvironmentArrayLen ()));
    for (size_t i = 0; i < envVector.size (); ++i)
    {
        char const *element = envVector[i];
        os << "element #" << i << " : "
           << (element ? std::string (envVector[i]) :
                         nullTerminator) << " "
           << std::endl;
    }

    return os;
}

ycom::Environment
ytest::ValueTypeConstructor <ycom::Environment>::ConstructWithNonDefaultData ()
{
    return ycom::Environment (NonDefaultMockEnvp);
}

bool
ytest::ValueTypeConstructor <ycom::Environment>::HasNonDefaultConstructor ()
{
    return true;
}

bool
ytest::ValueTypeConstructor <ycom::Environment>::Mutate (ycom::Environment &e)
{
    e.insert (MockerEnvKey.c_str (),
              MockEnvValue.c_str ());
    return true;
}

bool
ytest::ValueTypeConstructor <ycom::Environment>::Overloaded ()
{
    return true;
}

typedef ::testing::Types <ycom::Environment> YComValueConformanceTypes;
INSTANTIATE_TYPED_TEST_CASE_P (CommandEnvironment,
                               ValueTypeConformance,
                               YComValueConformanceTypes);


class ProgramEnvironmentValueDefault :
    public ::testing::Test
{
    protected:

        ycom::Environment environment;
};

TEST_F (ProgramEnvironmentValueDefault, OneElementOnConstruction)
{
    ycom::Environment environment;
    EXPECT_EQ (1, environment.underlyingEnvironmentArrayLen ());
}

TEST_F (ProgramEnvironmentValueDefault, FirstElementIsNullOnConstruction)
{
    ycom::Environment environment;
    EXPECT_THAT (environment.underlyingEnvironmentArray ()[0], IsNull ());
}

class ProgramEnvironmentValueNonDefault :
    public ::testing::Test
{
    public:

        ProgramEnvironmentValueNonDefault () :
            environment (NonDefaultMockEnvp)
        {
        }

    protected:

        ycom::Environment environment;
};

TEST_F (ProgramEnvironmentValueNonDefault, TwoElementsOnConstruction)
{
    EXPECT_EQ (2, environment.underlyingEnvironmentArrayLen ());
}

TEST_F (ProgramEnvironmentValueNonDefault, FirstElementMatchesEnvp)
{
    EXPECT_THAT (environment.underlyingEnvironmentArray ()[0],
            StrEq (NonDefaultMockEnvp[0]));
}

TEST_F (ProgramEnvironmentValueNonDefault, SecondElementIsNullTerminator)
{
    EXPECT_THAT (environment.underlyingEnvironmentArray ()[1],
                 IsNull ());
}

TEST_F (ProgramEnvironmentValueNonDefault, InsertNewBeforeNullTerminator)
{
    std::string const ExpectedValue (MockerEnvKey + "=" + MockEnvValue);

    environment.insert (MockerEnvKey.c_str (), MockEnvValue.c_str ());
    ASSERT_EQ (3, environment.underlyingEnvironmentArrayLen ());
    EXPECT_THAT (environment.underlyingEnvironmentArray ()[1],
                 StrEq (ExpectedValue));
}

TEST_F (ProgramEnvironmentValueNonDefault, LastElementIsNullTerminator)
{
    environment.insert (MockerEnvKey.c_str (), MockEnvValue.c_str ());
    ASSERT_EQ (3, environment.underlyingEnvironmentArrayLen ());
    EXPECT_THAT (environment.underlyingEnvironmentArray ()[2],
                 IsNull ());
}
