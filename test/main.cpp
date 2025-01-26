// define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #include <doctest/doctest.h>

#include <extl/expected/unexpected.hpp>
#include <extl/expected/expected.hpp>

#include <iostream>
#include <string>

void test_and_then() {
    // Case 1: has_value() == true, transform value
    extl::expected<int, std::string> e1(42);
    auto e2 = e1.and_then([](int val) {
        return extl::expected<std::string, std::string>(extl::in_place, std::to_string(val));
        });
    assert(e2.has_value() && e2.value() == "42");

    // Case 2: has_value() == false, propagate error
    extl::expected<int, std::string> e3(extl::unexpect, "error1");
    auto e4 = e3.and_then([](int) {
        return extl::expected<std::string, std::string>(extl::in_place, "should not run");
        });
    assert(!e4.has_value() && e4.error() == "error1");

    // Case 3: different reference types
    const extl::expected<int, std::string> e5(24);
    auto e6 = e5.and_then([](int val) {
        return extl::expected<std::string, std::string>(extl::in_place, "const: " + std::to_string(val));
        });
    assert(e6.has_value() && e6.value() == "const: 24");

    auto e7 = move(e5).and_then([](int val) {
        return extl::expected<std::string, std::string>(extl::in_place, "moved: " + std::to_string(val));
        });
    assert(e7.has_value() && e7.value() == "moved: 24");

    // Case 4: noexcept checks
    extl::expected<int, float> e8(42);
    static_assert(noexcept(e8.and_then([](int val) noexcept {
        return extl::expected<int, float>(extl::in_place, 2 * val);
        })), "and_then should be noexcept when F is noexcept");

    std::cout << "and_then tests passed!" << std::endl;
}

void test_transform() {
    // Case 1: has_value() == true, transform value
    extl::expected<int, std::string> e1(42);
    auto e2 = e1.transform([](int val) {
        return val * 2;
        });
    assert(e2.has_value() && e2.value() == 84);

    // Case 2: has_value() == false, propagate error
    extl::expected<int, std::string> e3(extl::unexpect, "error1");
    auto e4 = e3.transform([](int) {
        return 0; // should not run
        });
    assert(!e4.has_value() && e4.error() == "error1");

    // Case 3: value transforming to void
    auto e5 = e1.transform([](int val) {
        std::cout << "Value: " << val << std::endl;
        });
    assert(e5.has_value());

    // Case 4: different reference types
    const extl::expected<int, std::string> e6(24);
    auto e7 = e6.transform([](int val) {
        return val + 1;
        });
    assert(e7.has_value() && e7.value() == 25);

    auto e8 = move(e6).transform([](int val) {
        return val - 1;
        });
    assert(e8.has_value() && e8.value() == 23);

    // Case 5: noexcept checks
    extl::expected<int, float> e9(42);
    static_assert(noexcept(e9.transform([](int val) noexcept {
        return val * 2;
        })), "transform should be noexcept when F is noexcept");

    std::cout << "transform tests passed!" << std::endl;
}

int main() {
    test_and_then();
    test_transform();
    return 0;
}