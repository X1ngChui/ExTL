#include <doctest/doctest.h>

#include <result/status.hpp>

namespace extest {
    // Custom wrapper for a type that holds a double
    struct double_wrapper {
        double value;

        explicit double_wrapper(double val) noexcept : value(val) {}

        // Explicit conversion to int
        explicit operator int() const noexcept {
            return static_cast<int>(value);  // Explicit narrowing conversion
        }
    };

    // Custom wrapper for a type that holds an int
    struct int_wrapper {
        int value;

        explicit int_wrapper(int val) noexcept : value(val) {}

        // Explicit conversion from double_wrapper to int_wrapper
        explicit int_wrapper(const double_wrapper& dw) noexcept : value(static_cast<int>(dw.value)) {}
    };

    TEST_SUITE("extl::status") {
        TEST_CASE("DefaultConstructor") {
            extl::status<int> s;
            CHECK(s.ok());  // The status object constructed by default should not contain an error
        }

        TEST_CASE("NullerrConstructor") {
            extl::status<int> s(extl::nullerr);
            CHECK(s.ok());  // The status object constructed using nullerr should not contain an error
        }

        TEST_CASE("CopyConstructor_NoError") {
            extl::status<int> s1;
            extl::status<int> s2(s1);  // Copy construct an object that has no error
            CHECK(s2.ok());  // The copied status object should not contain an error
        }

        TEST_CASE("CopyConstructor_WithError") {
            extl::status<int> s1(extl::in_place, 42);  // Create a status object with an error
            extl::status<int> s2(s1);  // Copy construct a status object that contains an error
            CHECK_FALSE(s2.ok());  // The copied status object should contain an error
            CHECK(s2.error() == 42);  // The error value should be 42
        }

        TEST_CASE("MoveConstructor_NoError") {
            extl::status<int> s1;
            extl::status<int> s2(std::move(s1));  // Move construct a status object that has no error
            CHECK(s2.ok());  // The moved status object should not contain an error
        }

        TEST_CASE("MoveConstructor_WithError") {
            extl::status<int> s1(extl::in_place, 99);  // Create a status object with an error
            extl::status<int> s2(std::move(s1));  // Move construct a status object that contains an error
            CHECK_FALSE(s2.ok());  // The moved status object should contain an error
            CHECK(s2.error() == 99);  // The error value should be 99
        }

        TEST_CASE("ConvertingCopyConstructor") {
            double_wrapper dw(3.14);  // Create a status object with a double error
            extl::status<int_wrapper> s1(extl::in_place, dw);  // Construct with double_wrapper
            extl::status<int_wrapper> s2(s1);  // Copy construct with a different error type
            CHECK_FALSE(s2.ok());  // The copied status object should contain an error
            CHECK(s2.error().value == 3);  // The error value should be the int-converted value of 3.14
        }

        TEST_CASE("ConvertingMoveConstructor") {
            double_wrapper dw(3.14);  // Create a status object with a double error
            extl::status<int_wrapper> s1(extl::in_place, dw);  // Construct with double_wrapper
            extl::status<int_wrapper> s2(std::move(s1));  // Move construct with a different error type
            CHECK_FALSE(s2.ok());  // The moved status object should contain an error
            CHECK(s2.error().value == 3);  // The error value should be the int-converted value of 3.14
        }

        TEST_CASE("InPlaceConstructor") {
            double_wrapper dw(100.99);
            extl::status<double_wrapper> s(extl::in_place, dw);  // Create a status object with an error initialized using in_place_t
            CHECK_FALSE(s.ok());  // The status object should contain an error
            CHECK(s.error().value == 100.99);  // The error value should be 100.99
        }

        TEST_CASE("ErrorTypeConstructor") {
            extl::status<int> s(42);  // Create a status object using an error value directly
            CHECK_FALSE(s.ok());  // The status object should contain an error
            CHECK(s.error() == 42);  // The error value should be 42
        }

        TEST_CASE("AlwaysOkStatus") {
            extl::always_ok_status s;
            CHECK(s.ok());  // always_ok_status should always indicate no error
            CHECK(static_cast<bool>(s));  // The boolean conversion should return true (no error)
        }

        TEST_CASE("DestructorNoError") {
            extl::status<int> s;
            // A status object with no error should not do any special cleanup during destruction
            // No additional checks are needed here
        }

        TEST_CASE("DestructorWithError") {
            extl::status<int> s(extl::in_place, 77);
            // The status object was initialized with an error, so its error should be destroyed during destruction
            // No additional checks are needed here
        }
    }
}