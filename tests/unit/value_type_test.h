/*
 * value_type_test.h:
 * A simple typed test for ensuring that value types behave in a sane way,
 * namely that:
 * 0. The value type has a default parameterless constructor
 *    the meets the nothrow guaruntee
 * 1. Copy construction meets the nothrow guaruntee
 * 2. Copy assignment meets the nothrow guaruntee
 * 3. Copy construction creates an exact copy of the object
 *    verified with operator==
 * 4. Copy construction creates an completely independent
 *    copy of the object, verified by creating two copies
 *    of the object, and destroying the first, and verifying
 *    with operator==
 * 5. Copy assignment creates an exact copy of the object
 *    verified with operator==
 * 6. Copy assignment creates an completely independent
 *    copy of the object, verified by creating two copies
 *    of the object, and destroying the first, and verifying
 *    with operator==
 * 7. operator== and operator!= are Associative
 * 8. The type implements swap () and swap is Associative
 * 9. The type implements a move constructor and move
 *    constructor invalidates the old object (tested by
 *    inequality from moved-to and moved-from)
 *
 * See LICENCE.md for Copyright information
 */

#ifndef YIQI_VALUE_TYPE_TEST_H
#define YIQI_VALUE_TYPE_TEST_H

#include <memory>

#include <gtest/gtest.h>

using ::testing::ExitedWithCode;

namespace yiqi
{
    namespace testing
    {
        /**
        * @class ValueTypeConstructor
        * @brief tests which wish to verify their value types with
        * ValueTypeConformance should specialize this template and
        * overload ConstructWithNonDefaultData to create a copy of
        * ValueType with non-default data, and if applicable, overload
        * Mutate to change the data within the value
        */
        template <typename ValueType>
        class ValueTypeConstructor
        {
            public:

                ValueTypeConstructor () = default;

                /**
                 * @brief ConstructWithNonDefaultData
                 * @return should return a ValueType differing to the ValueType
                 * returned by the default constructor
                 */
                ValueType
                ConstructWithNonDefaultData ();

                /**
                 * @brief HasNonDefaultConstructor
                 * @return true if the value can be constructed as non-default
                 * directly, false if otherwise
                 */
                bool
                HasNonDefaultConstructor ();

                /**
                 * @brief Mutate
                 * @param value to be changed
                 * @return true if the value was altered, false otherwise
                 */
                bool
                Mutate (ValueType &value);

                /**
                 * @brief Overloaded
                 * @return MUST return true, as the
                 * template MUST be specialized
                 */
                bool
                Overloaded ();

            private:

                ValueTypeConstructor (ValueTypeConstructor const &) = delete;
                ValueTypeConstructor &
                operator= (ValueTypeConstructor const &) = delete;

        };

        template <typename ValueType>
        ValueType
        ValueTypeConstructor <ValueType>::ConstructWithNonDefaultData ()
        {
            return ValueType ();
        }

        template <typename ValueType>
        bool ValueTypeConstructor <ValueType>::HasNonDefaultConstructor ()
        {
            return false;
        }

        template <typename ValueType>
        bool ValueTypeConstructor <ValueType>::Mutate (ValueType &value)
        {
            return false;
        }

        template <typename ValueType>
        bool ValueTypeConstructor <ValueType>::Overloaded ()
        {
            return false;
        }

        namespace internal
        {
            template <typename ValueType>
            bool HasNonDefaultConstructor ()
            {
                ValueTypeConstructor <ValueType> constructor;
                return constructor.HasNonDefaultConstructor ();
            }

            template <typename ValueType>
            ValueType CreateNonDefault ()
            {
                ValueTypeConstructor <ValueType> constructor;
                return constructor.ConstructWithNonDefaultData ();
            }

            template <typename ValueType>
            bool Mutate (ValueType &value)
            {
                ValueTypeConstructor <ValueType> constructor;
                return constructor.Mutate (value);
            }

            char const *ValuesUnequalMessage = "Values are unequal";
        }
    }
}

/**
 * @brief a test fixture for testing the conformance of value-types
 * to value-type concepts.
 *
 * Tests that wish to implement this fixture MUST provide a
 * specialization for yiqi::testing::ValueTypeConstructor in the
 * yiqi::testing namespace
 */
template <typename ValueType>
class ValueTypeConformance :
    public ::testing::Test
{
    public:

        void SetUp ()
        {
            yiqi::testing::ValueTypeConstructor <ValueType> constructor;
            ASSERT_TRUE (constructor.Overloaded ());
        }

        ValueType initialValue;
};

template <typename ValueType>
class ValueTypeConformanceDeathTest :
    public ValueTypeConformance <ValueType>
{
};

TYPED_TEST_CASE_P (ValueTypeConformance);
TYPED_TEST_CASE_P (ValueTypeConformanceDeathTest);

/* If an exception is thrown during the construction of
 * initialValue then this test will fail */
TYPED_TEST_P (ValueTypeConformance, HasDefaultParameterlessConstructorNoThrow)
{
}

TYPED_TEST_P (ValueTypeConformance, NonDefaultUnequalToDefault)
{
    namespace yti = yiqi::testing::internal;

    TypeParam nonDefaultValue (yti::CreateNonDefault <TypeParam> ());

    const bool immutable = (!yti::HasNonDefaultConstructor <TypeParam> ()) &&
                           (!yti::Mutate (nonDefaultValue));

    if (immutable)
        EXPECT_EQ (nonDefaultValue, this->initialValue);
    else
        EXPECT_NE (nonDefaultValue, this->initialValue);
}

TYPED_TEST_P (ValueTypeConformance, CopyConstructionMeetsNoThrowGuaruntee)
{
    EXPECT_NO_THROW ({
        TypeParam value (this->initialValue);
    });
}

TYPED_TEST_P (ValueTypeConformance, CopyAssignmentMeetsNoThrowGuaruntee)
{
    EXPECT_NO_THROW ({
        TypeParam value;
        value = this->initialValue;
    });
}

TYPED_TEST_P (ValueTypeConformance, CopyConstructionCreatesExactCopy)
{
    TypeParam value (this->initialValue);
    EXPECT_EQ (value, this->initialValue);
}

TYPED_TEST_P (ValueTypeConformance, CopyAssignmentCreatesExactCopy)
{
    TypeParam value;
    value = this->initialValue;
    EXPECT_EQ (value, this->initialValue);
}

TYPED_TEST_P (ValueTypeConformance, CopyConstructionCreatesIndependentCopy)
{
    namespace ytesti = yiqi::testing::internal;

    TypeParam value (this->initialValue);
    if (ytesti::Mutate (value))
        EXPECT_NE (this->initialValue, value);
    else
        EXPECT_EQ (this->initialValue, value);

    EXPECT_NE (this->initialValue, value);
}

TYPED_TEST_P (ValueTypeConformance, CopyAssignmentCreatesIndependentCopy)
{
    namespace ytesti = yiqi::testing::internal;

    TypeParam value;
    value = this->initialValue;

    if (ytesti::Mutate (value))
        EXPECT_NE (this->initialValue, value);
    else
        EXPECT_EQ (this->initialValue, value);
}

TYPED_TEST_P (ValueTypeConformanceDeathTest, SurviveCopyConstruction)
{
    namespace ytesti = yiqi::testing::internal;

    TypeParam *firstValue (new TypeParam ());
    TypeParam secondValue (*firstValue);
    TypeParam thirdValue (secondValue);

    delete firstValue;

    /* If secondValue was dependent on firstValue, eg,
     * dangling pointer, then this test will either crash,
     * fail, or yield errors in a memory checker */
    EXPECT_EXIT ({
        if (thirdValue != secondValue)
        {
            std::cerr << ytesti::ValuesUnequalMessage;
            exit (1);
        }
        else
            ::exit (0);
    }, ExitedWithCode (0), "");
}

TYPED_TEST_P (ValueTypeConformanceDeathTest, SurviveCopyAssignment)
{
    namespace ytesti = yiqi::testing::internal;

    TypeParam *firstValue (new TypeParam ());
    TypeParam secondValue = *firstValue;
    TypeParam thirdValue;

    thirdValue = secondValue;

    delete firstValue;

    /* If secondValue was dependent on firstValue, eg,
     * dangling pointer, then this test will either crash,
     * fail, or yield errors in a memory checker */
    EXPECT_EXIT ({
        if (thirdValue != secondValue)
        {
            std::cerr << ytesti::ValuesUnequalMessage;
            exit (1);
        }
        else
            ::exit (0);
    }, ExitedWithCode (0), "");
}

TYPED_TEST_P (ValueTypeConformance, AssociativeEqualityOperators)
{
    TypeParam value (this->initialValue);
    EXPECT_TRUE ((value == this->initialValue) ==
                 (this->initialValue == value));
}

TYPED_TEST_P (ValueTypeConformance, AssociativeInequalityOperators)
{
    TypeParam value (this->initialValue);
    EXPECT_TRUE ((value != this->initialValue) ==
                 (this->initialValue != value));
}

TYPED_TEST_P (ValueTypeConformance, InverseInequalityAndEqualityOperators)
{
    TypeParam value (this->initialValue);
    EXPECT_TRUE ((value != this->initialValue) !=
                 (this->initialValue == value));
}

TYPED_TEST_P (ValueTypeConformance, ImplementsSwapRetainsEquality)
{
    using std::swap;

    TypeParam value (this->initialValue);
    bool      initialEquality = (value == this->initialValue);

    swap (value, this->initialValue);

    EXPECT_EQ (initialEquality, value == this->initialValue);
}

TYPED_TEST_P (ValueTypeConformance, ImplementsSwapRetainsInequality)
{
    using std::swap;

    TypeParam value (this->initialValue);
    bool      initialInequality = (value != this->initialValue);

    swap (value, this->initialValue);

    EXPECT_EQ (initialInequality, value != this->initialValue);
}

TYPED_TEST_P (ValueTypeConformance, MoveConstructorInvalidatesOldValue)
{
    TypeParam value (std::move (this->initialValue));

    EXPECT_NE (value, this->initialValue);
}

REGISTER_TYPED_TEST_CASE_P (ValueTypeConformance,
                            NonDefaultUnequalToDefault,
                            HasDefaultParameterlessConstructorNoThrow,
                            CopyConstructionMeetsNoThrowGuaruntee,
                            CopyAssignmentMeetsNoThrowGuaruntee,
                            CopyConstructionCreatesExactCopy,
                            CopyAssignmentCreatesExactCopy,
                            CopyConstructionCreatesIndependentCopy,
                            CopyAssignmentCreatesIndependentCopy,
                            AssociativeEqualityOperators,
                            AssociativeInequalityOperators,
                            InverseInequalityAndEqualityOperators,
                            ImplementsSwapRetainsEquality,
                            ImplementsSwapRetainsInequality,
                            MoveConstructorInvalidatesOldValue);

REGISTER_TYPED_TEST_CASE_P (ValueTypeConformanceDeathTest,
                            SurviveCopyConstruction,
                            SurviveCopyAssignment);

#endif // YIQI_VALUE_TYPE_TEST_H
