#include <iostream>
#include <string>
#include <cassert>
#include "ExTL/expected.h"

namespace extl {
    void test_expected_with_success() {
        expected<int, std::string> e(42);
        assert(e.has_value());
        assert(static_cast<bool>(e));
        assert(e.value() == 42);

        std::cout << "test_expected_with_success passed.\n";
    }

    void test_expected_with_error() {
        unexpected<std::string> err("Error occurred");
        expected<int, std::string> e(std::move(err));
        assert(!e.has_value());
        assert(!static_cast<bool>(e));
        assert(e.error() == "Error occurred");

        std::cout << "test_expected_with_error passed.\n";
    }

    void test_move_semantics() {
        expected<int, std::string> e1(42);
        assert(e1.has_value());
        assert(e1.value() == 42);

        expected<int, std::string> e2(std::move(e1));
        assert(e2.has_value());
        assert(e2.value() == 42);

        unexpected<std::string> err("Error occurred");
        expected<int, std::string> e3(std::move(err));
        assert(!e3.has_value());
        assert(e3.error() == "Error occurred");

        expected<int, std::string> e4(std::move(e3));
        assert(!e4.has_value());
        assert(e4.error() == "Error occurred");

        std::cout << "test_move_semantics passed.\n";
    }

    void test_custom_handler() {
        struct custom_handler {
            void operator()(std::string& error) noexcept {
                std::cerr << "Custom handler called with error: " << error << '\n';
            }
        };

        expected<int, std::string, custom_handler> e(unexpected<std::string>("Critical error"));
        assert(!e.has_value());
        e.value();

        std::cout << "test_custom_handler passed.\n";
    }
}

int main() {
    extl::test_expected_with_success();
    extl::test_expected_with_error();
    extl::test_move_semantics();
    extl::test_custom_handler();

    std::cout << "All tests passed.\n";
    return 0;
}
