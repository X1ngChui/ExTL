#pragma once
#include <type_traits>

namespace extl {
    template <typename Type, template <typename...> typename Template>
    constexpr bool is_specialization_of_v = false;
    template <template <typename...> typename Template, typename... Types>
    constexpr bool is_specialization_of_v<Template<Types...>, Template> = true;

    template <typename T>
    constexpr bool strict_default_constructible_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_default_constructible_v<T> : std::is_default_constructible_v<T>;

    template <typename T>
    constexpr bool strict_copy_constructible_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_copy_constructible_v<T> : std::is_copy_constructible_v<T>;

    template <typename T>
    constexpr bool strict_move_constructible_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_move_constructible_v<T> : std::is_move_constructible_v<T>;

    template <typename T, typename... Args>
    constexpr bool strict_constructible_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_constructible_v<T, Args...> : std::is_constructible_v<T, Args...>;

    template <typename T>
    constexpr bool strict_destructible_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_destructible_v<T> : std::is_destructible_v<T>;

    template <typename T>
    constexpr bool strict_copy_assignable_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_copy_assignable_v<T> : std::is_copy_assignable_v<T>;

    template <typename T>
    constexpr bool strict_move_assignable_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_move_assignable_v<T> : std::is_move_assignable_v<T>;

    template <typename From, typename To>
    constexpr bool strict_assignable_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_assignable_v<From, To> : std::is_assignable_v<From, To>;

    template <typename From, typename To>
    constexpr bool strict_convertible_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_convertible_v<From, To> : std::is_convertible_v<From, To>;

    template <typename T>
    constexpr bool strict_swappable_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_swappable_v<T> : std::is_swappable_v<T>;

    template <typename T, typename... Args>
    constexpr bool strict_invocable_v = EXTL_STRICT_CONSTRAINTS ? std::is_nothrow_invocable_v<T, Args...> : std::is_invocable_v<T, Args...>;
}