#include <doctest/doctest.h>

#include <extl/expected/expected.hpp>
#include <extl/expected/unexpected.hpp>

namespace extest {
    using namespace extl;
    using extl::unexpected;

    TEST_SUITE("expected<T, E>") {
        TEST_CASE("default constructor") {
            expected<int, char> e;
            CHECK(e.has_value());
            CHECK_EQ(e.value(), 0);
        }

        TEST_CASE("copy constructor with an expected value") {
            expected<int, char> src(42);
            expected<int, char> dest(src);
            CHECK(dest.has_value());
            CHECK_EQ(dest.value(), 42);
        }

        TEST_CASE("copy constructor with an unexpected value") {
            expected<int, char> src(unexpected('a'));
            expected<int, char> dest(src);
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("move constructor with an expected value") {
            expected<int, char> src(42);
            expected<int, char> dest(std::move(src));
            CHECK(dest.has_value());
            CHECK_EQ(dest.value(), 42);
        }

        TEST_CASE("move constructor with an unexpected value") {
            expected<int, char> src(unexpected('a'));
            expected<int, char> dest(std::move(src));
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("copy constructor with an expected value from a different type") {
            expected<int, char> src(42);
            expected<long, unsigned char> dest(src);
            CHECK(dest.has_value());
            CHECK_EQ(dest.value(), 42);
        }

        TEST_CASE("copy constructor with an unexpected value from a different type") {
            expected<int, char> src(unexpected('a'));
            expected<long, unsigned char> dest(src);
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("move constructor with an expected value from a different type") {
            expected<int, char> src(42);
            expected<long, unsigned char> dest(std::move(src));
            CHECK(dest.has_value());
            CHECK_EQ(dest.value(), 42);
        }

        TEST_CASE("move constructor with an unexpected value from a different type") {
            expected<int, char> src(unexpected('a'));
            expected<long, unsigned char> dest(std::move(src));
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("copy constructor with an expected value from a different type with a bool value") {
            expected<bool, char> src(true);
            expected<bool, unsigned char> dest(src);
            CHECK(dest.has_value());
            CHECK_EQ(dest.value(), true);
        }

        TEST_CASE("copy constructor with an unexpected value from a different type with a bool value") {
            expected<bool, char> src(unexpected('a'));
            expected<bool, unsigned char> dest(src);
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("move constructor with an expected value from a different type with a bool value") {
            expected<bool, char> src(true);
            expected<bool, unsigned char> dest(std::move(src));
            CHECK(dest.has_value());
            CHECK_EQ(dest.value(), true);
        }

        TEST_CASE("move constructor with an unexpected value from a different type with a bool value") {
            expected<bool, char> src(unexpected('a'));
            expected<bool, unsigned char> dest(std::move(src));
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("constructor with an expected value") {
            expected<int, char> e(42);
            CHECK(e.has_value());
            CHECK_EQ(e.value(), 42);
        }

        TEST_CASE("constructor with an unexpected value") {
            expected<int, char> e(unexpected('a'));
            CHECK_FALSE(e.has_value());
            CHECK_EQ(e.error(), 'a');
        }

        TEST_CASE("constructor with an expected value from a different type") {
            expected<long, unsigned char> e(42);
            CHECK(e.has_value());
            CHECK_EQ(e.value(), 42);
        }

        TEST_CASE("constructor with an unexpected value from a different type") {
            expected<long, unsigned char> e(unexpected('a'));
            CHECK_FALSE(e.has_value());
            CHECK_EQ(e.error(), 'a');
        }

        TEST_CASE("constructor with an expected value in place") {
            expected<int, char> e(in_place, 42);
            CHECK(e.has_value());
            CHECK_EQ(e.value(), 42);
        }

        TEST_CASE("constructor with an unexpected value in place") {
            expected<int, char> e(unexpect, 'a');
            CHECK_FALSE(e.has_value());
            CHECK_EQ(e.error(), 'a');
        }

        TEST_CASE("operator*") {
            expected<int, char> e(42);
            CHECK_EQ(*e, 42);
        }

        TEST_CASE("operator->") {
            struct S {
                int value;
            };
            expected<S, char> e(S{ 42 });
            CHECK_EQ(e->value, 42);
        }

        TEST_CASE("operator bool") {
            expected<int, char> e(42);
            CHECK(e);
        }

        TEST_CASE("observers") {
            expected<int, char> success(42);
            CHECK(success.has_value());
            CHECK_EQ(success.value(), 42);
            CHECK_EQ(*success, 42);
            CHECK(success);

            expected<int, char> failure(unexpected('a'));
            CHECK_FALSE(failure.has_value());
            CHECK_EQ(failure.error(), 'a');
            CHECK_FALSE(failure);
        }

        TEST_CASE("observers with default value") {
            expected<int, char> success(42);
            CHECK_EQ(success.value_or(0), 42);
            CHECK_EQ(success.error_or('b'), 'b');
            expected<int, char> failure(unexpected('a'));
            CHECK_EQ(failure.value_or(0), 0);
            CHECK_EQ(failure.error_or('b'), 'a');
        }

        TEST_CASE("and then") {
            expected<int, char> success(42);
            auto f = [](int value) noexcept { return expected<int, char>(value * 2); };
            auto result = success.and_then(f);
            CHECK(result.has_value());
            CHECK_EQ(result.value(), 84);
            expected<int, char> failure(unexpected('a'));
            auto g = [](int value) noexcept { return expected<int, char>(value * 2); };
            auto result2 = failure.and_then(g);
            CHECK_FALSE(result2.has_value());
            CHECK_EQ(result2.error(), 'a');
        }

        TEST_CASE("or else") {
            expected<int, char> success(42);
            auto f = [](char error) noexcept { return expected<int, char>(error); };
            auto result = success.or_else(f);
            CHECK(result.has_value());
            CHECK_EQ(result.value(), 42);
            expected<int, char> failure(unexpected('a'));
            auto g = [](char error) noexcept { return expected<int, char>(error); };
            auto result2 = failure.or_else(g);
            CHECK(result2.has_value());
            CHECK_EQ(result2.value(), 'a');
        }

        TEST_CASE("transform") {
            expected<int, char> success(42);
            auto f = [](int value) noexcept { return value * 2; };
            auto result = success.transform(f);
            CHECK(result.has_value());
            CHECK_EQ(result.value(), 84);
            expected<int, char> failure(unexpected('a'));
            auto g = [](int value) noexcept { return value * 2; };
            auto result2 = failure.transform(g);
            CHECK_FALSE(result2.has_value());
            CHECK_EQ(result2.error(), 'a');
        }

        TEST_CASE("transform error") {
            expected<int, char> success(42);
            auto f = [](char error) noexcept { return error; };
            auto result = success.transform_error(f);
            CHECK(result.has_value());
            CHECK_EQ(result.value(), 42);
            expected<int, char> failure(unexpected('a'));
            auto g = [](char error) noexcept { return error; };
            auto result2 = failure.transform_error(g);
            CHECK_FALSE(result2.has_value());
            CHECK_EQ(result2.error(), 'a');
        }
    }

    TEST_SUITE("expected<void, E>") {
        TEST_CASE("default constructor") {
            expected<void, char> e;
            CHECK(e.has_value());
        }

        TEST_CASE("copy constructor with an unexpected value") {
            expected<void, char> src(unexpected('a'));
            expected<void, char> dest(src);
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("move constructor with an unexpected value") {
            expected<void, char> src(unexpected('a'));
            expected<void, char> dest(std::move(src));
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("copy constructor with an unexpected value from a different type") {
            expected<void, char> src(unexpected('a'));
            expected<void, unsigned char> dest(src);
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("move constructor with an unexpected value from a different type") {
            expected<void, char> src(unexpected('a'));
            expected<void, unsigned char> dest(std::move(src));
            CHECK_FALSE(dest.has_value());
            CHECK_EQ(dest.error(), 'a');
        }

        TEST_CASE("constructor with an unexpected value") {
            expected<void, char> e(unexpected('a'));
            CHECK_FALSE(e.has_value());
            CHECK_EQ(e.error(), 'a');
        }

        TEST_CASE("constructor with an unexpected value from a different type") {
            expected<void, unsigned char> e(unexpected('a'));
            CHECK_FALSE(e.has_value());
            CHECK_EQ(e.error(), 'a');
        }

        TEST_CASE("constructor with an unexpected value in place") {
            expected<void, char> e(unexpect, 'a');
            CHECK_FALSE(e.has_value());
            CHECK_EQ(e.error(), 'a');
        }

        TEST_CASE("operator bool") {
            expected<void, char> e;
            CHECK(e);
        }

        TEST_CASE("observers") {
            expected<void, char> success;
            CHECK(success.has_value());
            CHECK(success);
            expected<void, char> failure(unexpected('a'));
            CHECK_FALSE(failure.has_value());
            CHECK_EQ(failure.error(), 'a');
            CHECK_FALSE(failure);
        }

        TEST_CASE("observers with default value") {
            expected<void, char> success;
            CHECK_EQ(success.error_or('b'), 'b');
            expected<void, char> failure(unexpected('a'));
            CHECK_EQ(failure.error_or('b'), 'a');
        }

        TEST_CASE("and then") {
            expected<void, char> success;
            auto f = []() noexcept { return expected<void, char>(); };
            auto result = success.and_then(f);
            CHECK(result.has_value());
            expected<void, char> failure(unexpected('a'));
            auto g = []() noexcept { return expected<void, char>(); };
            auto result2 = failure.and_then(g);
            CHECK_FALSE(result2.has_value());
            CHECK_EQ(result2.error(), 'a');
        }

        TEST_CASE("or else") {
            expected<void, char> success;
            auto f = [](char error) noexcept { return expected<void, char>(unexpected(error)); };
            auto result = success.or_else(f);
            CHECK(result.has_value());
            expected<void, char> failure(unexpected('a'));
            auto g = [](char error) noexcept { return expected<void, char>(unexpected(error)); };
            auto result2 = failure.or_else(g);
            CHECK_FALSE(result2.has_value());
            CHECK_EQ(result2.error(), 'a');
        }

        TEST_CASE("transform") {
            expected<void, char> success;
            auto f = []() noexcept { return 42; };
            auto result = success.transform(f);
            CHECK(result.has_value());
            CHECK_EQ(result.value(), 42);
            expected<void, char> failure(unexpected('a'));
            auto g = []() noexcept { return 42; };
            auto result2 = failure.transform(g);
            CHECK_FALSE(result2.has_value());
            CHECK_EQ(result2.error(), 'a');
        }

        TEST_CASE("transform error") {
            expected<void, char> success;
            auto f = [](char error) noexcept { return error; };
            auto result = success.transform_error(f);
            CHECK(result.has_value());
            expected<void, char> failure(unexpected('a'));
            auto g = [](char error) noexcept { return error; };
            auto result2 = failure.transform_error(g);
            CHECK_FALSE(result2.has_value());
            CHECK_EQ(result2.error(), 'a');
        }
    }
}