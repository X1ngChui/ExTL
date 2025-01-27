#pragma once

#include <cassert>
#include <memory>
#include <extl/utility/type_traits.hpp>
#include <extl/expected/inplace.hpp>
#include <extl/expected/unexpected.hpp>

namespace extl {
    template <typename T, typename E>
    class expected;

    /*
     * Concept to check if a type is an `expected` type.
     */
    template <typename T>
    concept is_expected = is_specialization_of_v<T, expected>;

    struct unexpect_t { explicit unexpect_t() = default; };
    inline constexpr unexpect_t unexpect{};

    /**
     * The `expected` class represents a value that may or may not be present.
     * It holds either a value of type `T` or an error value of type `E`.
     *
     * @tparam T - The type of the value stored in the `expected` object.
     * @tparam E - The type of the error value stored in the `expected` object.
     */
    template <typename T, typename E>
    class expected {
    public:
        using value_type = T;
        using error_type = E;

        /*
         * Default constructor.
         * Enabled only if `T` is (nothrow) default constructible.
         */
        constexpr expected() noexcept(std::is_nothrow_default_constructible_v<T>)
            requires strict_default_constructible_v<T>
        : _value(), _has_value(true) {}

        /*
         * Copy constructor.
         * Enabled only if `T` and `E` are (nothrow) copy constructible.
         * 
         * @param other - The `expected` object to copy from.
         */
        constexpr expected(const expected& other)
            noexcept(std::is_nothrow_copy_constructible_v<T> and std::is_nothrow_copy_constructible_v<E>)
            requires strict_copy_constructible_v<T> and strict_copy_constructible_v<E>
        : _has_value(other.has_value())
        {
            if (_has_value) [[likely]]
                std::construct_at(std::addressof(_value), *other);
            else
                std::construct_at(std::addressof(_error), other.error());
        }

        /*
         * Move constructor.
         * Enabled only if `T` and `E` are (nothrow) move constructible.
         * 
         * @param other - The `expected` object to move from.
         */
        constexpr expected(expected&& other)
            noexcept(std::is_nothrow_move_constructible_v<T> and std::is_nothrow_move_constructible_v<E>)
            requires strict_move_constructible_v<T> and strict_move_constructible_v<E>
            : _has_value(other.has_value())
        {
            if (_has_value) [[likely]]
                std::construct_at(std::addressof(_value), std::move(*other));
            else
                std::construct_at(std::addressof(_error), std::move(other.error()));
        }
         
        /*
         * Copy constructor from a different `expected` type.
         * 
         * @tparam U - The type of the value stored in the `expected` object to copy from.
         *         G - The type of the error value stored in the `expected` object to copy from.
         * 
         * @param other - The `expected` object to copy from.
         *
         * Enabled if:
         * - `T` is (nothrow) constructible from `const U&`.
         * - `E` is (nothrow) constructible from `const G&`.
         * - If `T` is not `bool`, `T` is not constructible or convertible from `expected<U, G>`.
         * - `expected<U, G>` is not constructible from `unexpected<E>`.
         * 
         * Implicit only if `U` and `G` are convertible to `T` and `E`, respectively.
         */ 
        template <typename U, typename G>
        constexpr explicit(not std::is_convertible_v<const U&, T> or not std::is_convertible_v<const G&, E>) expected(const expected<U, G>& other) noexcept(std::is_nothrow_constructible_v<T, const U&> and std::is_nothrow_constructible_v<E, const G&>)
            requires strict_constructible_v<T, const U&> and 
                     strict_constructible_v<E, const G&> and (
                         std::is_same_v<bool, std::remove_cv_t<T>> or (
                             (not std::is_constructible_v<T, expected<U, G>&>) and
                             (not std::is_constructible_v<T, expected<U, G>>) and
                             (not std::is_constructible_v<T, const expected<U, G>&>) and
                             (not std::is_constructible_v<T, const expected<U, G>>) and
                             (not std::is_convertible_v<expected<U, G>&, T>) and
                             (not std::is_convertible_v<expected<U, G>, T>) and
                             (not std::is_convertible_v<const expected<U, G>&, T>) and
                             (not std::is_convertible_v<const expected<U, G>, T>)
                         )
                     ) and (
                         (not std::is_constructible_v<unexpected<E>, expected<U, G>&>) and
                         (not std::is_constructible_v<unexpected<E>, expected<U, G>>) and
                         (not std::is_constructible_v<unexpected<E>, const expected<U, G>&>) and
                         (not std::is_constructible_v<unexpected<E>, const expected<U, G>>)
                     )
            : _has_value(other.has_value())
        {
            if (_has_value) [[likely]]
                std::construct_at(std::addressof(_value), *other);
            else
                std::construct_at(std::addressof(_error), other.error());
        }

        /*
         * Move constructor from a different `expected` type.
         * 
         * @tparam U - The type of the value stored in the `expected` object to move from.
         *         G - The type of the error value stored in the `expected` object to move from.
         * 
         * @param other - The `expected` object to move from.
         *
         * Enabled if:
         * - `T` is (nothrow) constructible from `U&&`.
         * - `E` is (nothrow) constructible from `G&&`.
         * - If `T` is not `bool`, `T` is not constructible or convertible from `expected<U, G>`.
         * - `expected<U, G>` is not constructible from `unexpected<E>`.
         * 
         * Implicit only if `U` and `G` are convertible to `T` and `E`, respectively.
         */
        template <typename U, typename G>
        constexpr explicit(not std::is_convertible_v<U&&, T> or not std::is_convertible_v<G&&, E>) expected(expected<U, G>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&> and not std::is_nothrow_constructible_v<E, G&&>)
            requires strict_constructible_v<T, U&&> and
                     strict_constructible_v<E, G&&> and (
                         std::is_same_v<bool, std::remove_cv_t<T>> or (
                             (not std::is_constructible_v<T, expected<U, G>&>) and
                             (not std::is_constructible_v<T, expected<U, G>>) and
                             (not std::is_constructible_v<T, const expected<U, G>&>) and
                             (not std::is_constructible_v<T, const expected<U, G>>) and
                             (not std::is_convertible_v<expected<U, G>&, T>) and
                             (not std::is_convertible_v<expected<U, G>, T>) and
                             (not std::is_convertible_v<const expected<U, G>&, T>) and
                             (not std::is_convertible_v<const expected<U, G>, T>)
                         )
                     ) and (
                         (not std::is_constructible_v<unexpected<E>, expected<U, G>&>) and
                         (not std::is_constructible_v<unexpected<E>, expected<U, G>>) and
                         (not std::is_constructible_v<unexpected<E>, const expected<U, G>&>) and
                         (not std::is_constructible_v<unexpected<E>, const expected<U, G>>)
                     )
            : _has_value(other.has_value())
        {
            if (_has_value) [[likely]]
                std::construct_at(std::addressof(_value), std::move(*other));
            else
                std::construct_at(std::addressof(_error), std::move(other.error()));
        }

        /*
         * Constructor accepting a value of type `U` that `T` is constructible from.
         * 
         * @tparam U - The type of the value to construct the `expected` object with.
         * 
         * @param v - The value used to construct the `expected` object.
         * 
         * Enabled if:
         * - `U` is not `in_place_t` or `expected<T, E>`.
         * - `T` is (nothrow) constructible from `U`.
         * - `U` is not an `unexpected` type.
         * - If `T` is `bool`, `U` is not an `expected` type.
         * 
         * Implicit only if `U` is convertible to `T`.
         */
        template <typename U = T >
        constexpr explicit(not std::is_convertible_v<U, T>) expected(U&& v) noexcept(std::is_nothrow_constructible_v<T, U>)
            requires (not std::is_same_v<std::remove_cvref_t<U>, in_place_t>) and
                     (not std::is_same_v<expected<T, E>, std::remove_cvref_t<U>>) and
                     strict_constructible_v<T, U> and
                     (not is_unexpected<std::remove_cvref_t<U>>) and
                     ((not std::is_same_v<bool, std::remove_cv_t<T>>) or 
                      (not is_expected<std::remove_cvref_t<U>>))
            : _value(std::forward<U>(v)), _has_value(true) {}

        /*
         * Constructor accepting an error value of type `G` that `E` is constructible from.
         * 
         * @tparam G - The type of the error value to construct the `expected` object with.
         * 
         * @param e - The error value used to construct the `expected` object.
         * 
         * Enabled only if `E` is (nothrow) constructible from `E` 
         * 
         * Implicit only if `G` is convertible to `E`.
         */
        template <typename G>
        constexpr explicit(not std::is_convertible_v<const G&, E>) expected(const unexpected<G>& e) noexcept(std::is_nothrow_constructible_v<E, const G&>)
            requires strict_constructible_v<E, const G&>
        : _error(e.error()), _has_value(false) {}

        /*
         * Constructor accepting an error value of type `G` that `E` is constructible from.
         *
         * @tparam G - The type of the error value to construct the `expected` object with.
         *
         * @param e - The error value used to construct the `expected` object.
         *
         * Enabled only if `E` is (nothrow) constructible from `G`
         * 
         * Implicit only if `G` is convertible to `E`.
         */
        template <typename G>
        constexpr explicit(not std::is_convertible_v<G&&, E>) expected(unexpected<G>&& e) noexcept(std::is_nothrow_constructible_v<E, G&&>)
            requires strict_constructible_v<E, G&&>
        : _error(std::move(e.error())), _has_value(false) {}

        /*
         * Constructor for in-place construction of the value.
         * Accepts a variable number of arguments to construct `T` directly.
         *
         * @tparam Args - The types of the arguments used to construct the value in place.
         * 
         * @param (tag) - Tag to indicate in-place construction, use `extl::in_place`.
         *        args - The arguments used to construct the value in place.
         * 
         * Enabled if `T` is (nothrow) constructible from the provided arguments.
         *
         */
        template <typename... Args>
        constexpr explicit expected(in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
            requires strict_constructible_v<T, Args...>
        : _value(std::forward<Args>(args)...), _has_value(true) {}

        /*
         * Constructor for in-place construction of the error value.
         * Accepts a variable number of arguments to construct `E` directly.
         *
         * @tparam Args - The types of the arguments used to construct the error value in place.
         *
         * @param (tag) - Tag to indicate in-place error construction, use `extl::unexpect`.
         *        args - The arguments used to construct the error value in place.
         *
         * Enabled if `E` is (nothrow) constructible from the provided arguments.
         *
         */
        template <typename... Args>
        constexpr explicit expected(unexpect_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
            requires strict_constructible_v<E, Args...>
            : _error(std::forward<Args>(args)...), _has_value(false) {}

        /*
         * Destructor.
         * 
         * Enabled only if `T` and `E` are (nothrow) destructible.
         */
        ~expected() noexcept(std::is_nothrow_destructible_v<T> and std::is_nothrow_destructible_v<E>)
            requires strict_destructible_v<T> and strict_destructible_v<E>
        {
            if (has_value()) [[likely]]
                std::destroy_at(std::addressof(_value));
            else
                std::destroy_at(std::addressof(_error));
        }

        /*
         * `opeartor->` overload to access the value for const objects.
         * 
         * @return const T* - A const pointer to the value.
         */
        constexpr const T* operator->() const noexcept {
            assert(has_value());
            return std::addressof(_value);
        }
        /*
         * `opeartor->` overload to access the value for non-const objects.
         *
         * @return T* - A mutable pointer to the value.
         */
        constexpr T* operator->() noexcept {
            assert(has_value());
            return std::addressof(_value);
        }

        /*
         * operator* overload to access the value for const objects.
         *
         * @return const T& - A const reference to the value.
         */
        constexpr const T& operator*() const& noexcept {
            assert(has_value());
            return _value;
        }
        /*
         * operator* overload to access the value for non-const objects.
         *
         * @return T& - A mutable reference to the value.
         */
        constexpr T& operator*() & noexcept {
            assert(has_value());
            return _value;
        }
        /*
         * operator* overload to access the value for const rvalue objects.
         *
         * @return const T&& - A const rvalue reference to the value.
         */
        constexpr const T&& operator*() const&& noexcept {
            assert(has_value());
            return std::move(_value);
        }
        /*
         * operator* overload to access the value for rvalue objects.
         *
         * @return T&& - A mutable rvalue reference to the value.
         */
        constexpr T&& operator*() && noexcept {
            assert(has_value());
            return std::move(_value);
        }

        /*
         * operator bool overload to check if the `expected` object has a value.
         *
         * @return bool - `true` if the `expected` object has a value, `false` otherwise.
         */
        constexpr explicit operator bool() const noexcept { return _has_value; }
        /*
         * has_value() method to check if the `expected` object has a value.
         *
         * @return bool - `true` if the `expected` object has a value, `false` otherwise.
         */
        constexpr bool has_value() const noexcept { return _has_value; }

        /*
         * value() method to access the value for const objects.
         *
         * @return const T& - A const reference to the value.
         */
        constexpr const T& value() const& noexcept {
            assert(has_value());
            return _value;
        }
        /*
         * value() method to access the value for non-const objects.
         *
         * @return T& - A mutable reference to the value.
         */
        constexpr T& value() & noexcept {
            assert(has_value());
            return _value;
        }
        /*
         * value() method to access the value for const rvalue objects.
         *
         * @return const T&& - A const rvalue reference to the value.
         */
        constexpr const T&& value() const&& noexcept {
            assert(has_value());
            return std::move(_value);
        }
        /*
         * value() method to access the value for rvalue objects.
         *
         * @return T&& - A mutable rvalue reference to the value.
         */
        constexpr T&& value() && noexcept {
            assert(has_value());
            return std::move(_value);
        }

        /*
         * error() method to access the error value for const objects.
         *
         * @return const E& - A const reference to the error value.
         */
        constexpr const E& error() const& noexcept {
            assert(not has_value());
            return _error;
        }
        /*
         * error() method to access the error value for non-const objects.
         *
         * @return E& - A mutable reference to the error value.
         */
        constexpr E& error() & noexcept {
            assert(not has_value());
            return _error;
        }
        /*
         * error() method to access the error value for const rvalue objects.
         *
         * @return const E&& - A const rvalue reference to the error value.
         */
        constexpr const E&& error() const&& noexcept {
            assert(not has_value());
            return std::move(_error);
        }
        /*
         * error() method to access the error value for rvalue objects.
         *
         * @return E&& - A mutable rvalue reference to the error value.
         */
        constexpr E&& error() && noexcept {
            assert(not has_value());
            return std::move(_error);
        }

        /*
         * value_or() method to access the value or a default value for const objects.
         *
         * @tparam U - The type of the default value.
         *
         * @param default_value - The default value to return if the `expected` object does not have a value.
         *
         * @return T - The value if the `expected` object has a value, the default value otherwise.
         * 
         * Enabled only if `T` is (nothrow) copy constructible and `U` is (nothrow) convertible to `T`.
         */
        template <typename U>
        constexpr T value_or(U&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<T> and std::is_nothrow_convertible_v<U, T>)
            requires strict_copy_constructible_v<T> and strict_convertible_v<U, T>
        {
            if (has_value()) [[likely]]
                return **this;
            return static_cast<T>(std::forward<U>(default_value));
        }
        /*
         * value_or() method to access the value or a default value for non-const objects.
         *
         * @tparam U - The type of the default value.
         *
         * @param default_value - The default value to return if the `expected` object does not have a value.
         *
         * @return T - The value if the `expected` object has a value, the default value otherwise.
         *
         * Enabled only if `T` is (nothrow) move constructible and `U` is (nothrow) convertible to `T`.
         */
        template <typename U>
        constexpr T value_or(U&& default_value) && noexcept(std::is_nothrow_move_constructible_v<T> and std::is_nothrow_convertible_v<U, T>)
            requires strict_move_constructible_v<T> and strict_convertible_v<U, T>
        {
            if (has_value()) [[likely]]
                return std::move(**this);
            return static_cast<T>(std::forward<U>(default_value));
        }

        /*
         * error_or() method to access the error value or a default value for const objects.
         *
         * @tparam G - The type of the default value.
         *
         * @param default_value - The default value to return if the `expected` object does not have an error value.
         *
         * @return E - The error value if the `expected` object does not have a value, the default value otherwise.
         *
         * Enabled only if `E` is (nothrow) copy constructible and `G` is (nothrow) convertible to `E`.
         */
        template <typename G = E>
        constexpr E error_or(G&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<E> and std::is_nothrow_convertible_v<G, E>)
            requires strict_copy_constructible_v<E> and strict_convertible_v<G, E>
        {
            if (has_value()) [[likely]]
                return static_cast<E>(std::forward<G>(default_value));
            return error();
        }
        /*
         * error_or() method to access the error value or a default value for non-const objects.
         *
         * @tparam G - The type of the default value.
         *
         * @param default_value - The default value to return if the `expected` object does not have an error value.
         *
         * @return E - The error value if the `expected` object does not have a value, the default value otherwise.
         *
         * Enabled only if `E` is (nothrow) move constructible and `G` is (nothrow) convertible to `E`.
         */
        template <typename G = E>
        constexpr E error_or(G&& default_value) && noexcept(std::is_nothrow_move_constructible_v<E> and std::is_nothrow_convertible_v<G, E>)
            requires strict_move_constructible_v<E> and strict_convertible_v<G, E>
        {
            if (has_value()) [[likely]]
                return static_cast<E>(std::forward<G>(default_value));
            return std::move(error());
        }

        /*
         * and_then() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - The result of `F` is an `expected` object.
         * - The error type of the result of `F` is the same as `E`.
         * - `E` is (northrow) constructible from the error value of the result of `F`.
         */
        template <typename F>
        constexpr auto and_then(F&& f) & 
            noexcept(std::is_nothrow_invocable_v<F, decltype((**this))> and std::is_nothrow_constructible_v<E, decltype(error())>)
            requires strict_invocable_v<F, decltype((**this))> and 
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype((**this))>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype((**this))>>::error_type, E> and
                     strict_constructible_v<E, decltype(error())>
        {
            using U = std::remove_cvref_t<std::invoke_result_t<F, decltype((**this))>>;
            if (has_value()) [[likely]]
                return std::invoke(std::forward<F>(f), **this);
            return U(unexpect, error());
        }
        /*
         * and_then() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - The result of `F` is an `expected` object.
         * - The error type of the result of `F` is the same as `E`.
         * - `E` is (nothrow) constructible from the error value of the result of `F`.
         */
        template <typename F>
        constexpr auto and_then(F&& f) const& 
            noexcept(std::is_nothrow_invocable_v<F, decltype((**this))> and std::is_nothrow_constructible_v<E, decltype(error())>)
            requires strict_invocable_v<F, decltype((**this))> and 
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype((**this))>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype((**this))>>::error_type, E> and
                     strict_constructible_v<E, decltype(error())>
        {
            using U = std::remove_cvref_t<std::invoke_result_t<F, decltype((**this))>>;
            if (has_value()) [[likely]]
                return std::invoke(std::forward<F>(f), **this);
            return U(unexpect, error());
        }
        /*
         * and_then() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - The result of `F` is an `expected` object.
         * - The error type of the result of `F` is the same as `E`.
         * - `E` is (nothrow) constructible from the error value of the result of `F`.
         */
        template <typename F>
        constexpr auto and_then(F&& f) && 
            noexcept(std::is_nothrow_invocable_v<F, decltype(std::move(**this))> and std::is_nothrow_constructible_v<E, decltype(std::move(error()))>)
            requires strict_invocable_v<F, decltype(std::move(**this))> and
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(**this))>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(**this))>>::error_type, E> and
                     strict_constructible_v<E, decltype(std::move(error()))>
        {
            using U = std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(**this))>>;
            if (has_value()) [[likely]]
                return std::invoke(std::forward<F>(f), std::move(**this));
            return U(unexpect, std::move(error()));
        }
        /*
         * and_then() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - The result of `F` is an `expected` object.
         * - The error type of the result of `F` is the same as `E`.
         * - `E` is (nothrow) constructible from the error value of the result of `F`.
         */
        template <typename F>
        constexpr auto and_then(F&& f) const&& 
            noexcept(std::is_nothrow_invocable_v<F, decltype(std::move(**this))> and std::is_nothrow_constructible_v<E, decltype(std::move(error()))>)
            requires strict_invocable_v<F, decltype(std::move(**this))> and 
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(**this))>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(**this))>>::error_type, E> and
                     strict_constructible_v<E, decltype(std::move(error()))>
        {
            using U = std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(**this))>>;
            if (has_value()) [[likely]]
                return std::invoke(std::forward<F>(f), std::move(**this));
            return U(unexpect, std::move(error()));
        }

        /*
         * or_else() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - The result of `F` is an `expected` object.
         * - The value type of the result of `F` is the same as `T`.
         * - `T` is (nothrow) constructible from the value of the result of `F`.
         */
        template <typename F>
        constexpr auto or_else(F&& f) & noexcept(std::is_nothrow_invocable_v<F, decltype(error())> and
            std::is_nothrow_constructible_v<T, decltype((**this))>)
            requires strict_invocable_v<F, decltype(error())> and
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>::value_type, T> and
                     strict_constructible_v<T, decltype((**this))>

        {
            using G = std::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>;
            if (has_value()) [[likely]]
                return G(in_place, **this);
            return std::invoke(std::forward<F>(f), error());
        }
        /*
         * or_else() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - The result of `F` is an `expected` object.
         * - The value type of the result of `F` is the same as `T`.
         * - `T` is (nothrow) constructible from the value of the result of `F`.
         */
        template <typename F>
        constexpr auto or_else(F&& f) const& noexcept(std::is_nothrow_invocable_v<F, decltype(error())> and
            std::is_nothrow_constructible_v<T, decltype((**this))>)
            requires strict_invocable_v<F, decltype(error())> and
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>::value_type, T> and
                     strict_constructible_v<T, decltype((**this))>

        {
            using G = std::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>;
            if (has_value()) [[likely]]
                return G(in_place, **this);
            return std::invoke(std::forward<F>(f), error());
        }
        /*
         * or_else() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - The result of `F` is an `expected` object.
         * - The value type of the result of `F` is the same as `T`.
         * - `T` is (nothrow) constructible from the value of the result of `F`.
         */
        template <typename F>
        constexpr auto or_else(F&& f) && noexcept(std::is_nothrow_invocable_v<F, decltype(std::move(error()))> and
            std::is_nothrow_constructible_v<T, decltype(std::move(**this))>)
            requires strict_invocable_v<F, decltype(std::move(error()))> and
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(error()))>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(error()))>>::value_type, T> and
                     strict_constructible_v<T, decltype(std::move(**this))>

        {
            using G = std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(error()))>>;
            if (has_value()) [[likely]]
                return G(in_place, std::move(**this));
            return std::invoke(std::forward<F>(f), std::move(error()));
        }
        /*
         * or_else() method to chain a function that returns an `expected` object.
         *
         * @tparam F - The type of the function to chain.
         *
         * @param f - The function to chain.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - The result of `F` is an `expected` object.
         * - The value type of the result of `F` is the same as `T`.
         * - `T` is (nothrow) constructible from the value of the result of `F`.
         */
        template <typename F>
        constexpr auto or_else(F&& f) const&& noexcept(std::is_nothrow_invocable_v<F, decltype(std::move(error()))> and
            std::is_nothrow_constructible_v<T, decltype(std::move(**this))>)
            requires strict_invocable_v<F, decltype(std::move(error()))> and
                     is_expected<std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(error()))>>> and
                     std::is_same_v<typename std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(error()))>>::value_type, T> and
                     strict_constructible_v<T, decltype(std::move(**this))>

        {
            using G = std::remove_cvref_t<std::invoke_result_t<F, decltype(std::move(error()))>>;
            if (has_value()) [[likely]]
                return G(in_place, std::move(**this));
            return std::invoke(std::forward<F>(f), std::move(error()));
        }

        /*
         * transform() method to apply a function to the value.
         * 
         * @tparam F - The type of the function to apply.
         * 
         * @param f - The function to apply.
         * 
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         * 
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - If the result of `F` is not `void`, `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform(F&& f) & 
            noexcept(
                std::is_nothrow_invocable_v<F, decltype((**this))> and (
                    std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>> or
                    std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>, std::invoke_result_t<F, decltype((**this))>>
                ) and
                std::is_nothrow_constructible_v<E, decltype(error())>
            )
            requires strict_invocable_v<F, decltype((**this))> and (
                        std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>> or 
                        strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>, std::invoke_result_t<F, decltype((**this))>>
                     ) and strict_constructible_v<E, decltype(error())>
        {
            using U = std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>;
            if (has_value()) [[likely]] {
                if constexpr (std::is_void_v<U>) {
                    std::invoke(std::forward<F>(f), **this);
                    return expected<U, E>();
                } else {
                    return expected<U, E>(in_place, std::invoke(std::forward<F>(f), **this));
                }
            }
            return expected<U, E>(unexpect, error());
        }
        /*
         * transform() method to apply a function to the value.
         *
         * @tparam F - The type of the function to apply.
         *
         * @param f - The function to apply.
         *
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - If the result of `F` is not `void`, `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform(F&& f) const& 
            noexcept(
                std::is_nothrow_invocable_v<F, decltype((**this))> and (
                    std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>> or
                    std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>, std::invoke_result_t<F, decltype((**this))>>
                ) and
                std::is_nothrow_constructible_v<E, decltype(error())>
            )
            requires strict_invocable_v<F, decltype((**this))> and (
                        std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>> or 
                        strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>, std::invoke_result_t<F, decltype((**this))>>
                     ) and strict_constructible_v<E, decltype(error())>
        {
            using U = std::remove_cv_t<std::invoke_result_t<F, decltype((**this))>>;
            if (has_value()) [[likely]] {
                if constexpr (std::is_void_v<U>) {
                    std::invoke(std::forward<F>(f), **this);
                    return expected<U, E>();
                } else {
                    return expected<U, E>(in_place, std::invoke(std::forward<F>(f), **this));
                }
            }
            return expected<U, E>(unexpect, error());
        }
        /*
         * transform() method to apply a function to the value.
         *
         * @tparam F - The type of the function to apply.
         *
         * @param f - The function to apply.
         *
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - If the result of `F` is not `void`, `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform(F&& f) && 
            noexcept(
                std::is_nothrow_invocable_v<F, decltype(std::move(**this))> and (
                    std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>> or
                    std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>, std::invoke_result_t<F, decltype(std::move(**this))>>
                ) and
                std::is_nothrow_constructible_v<E, decltype(std::move(error()))>
            )
            requires strict_invocable_v<F, decltype(std::move(**this))> and (
                        std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>> or 
                        strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>, std::invoke_result_t<F, decltype(std::move(**this))>>
                     ) and strict_constructible_v<E, decltype(std::move(error()))>
        {
            using U = std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>;
            if (has_value()) [[likely]] {
                if constexpr (std::is_void_v<U>) {
                    std::invoke(std::forward<F>(f), std::move(**this));
                    return expected<U, E>();
                } else {
                    return expected<U, E>(in_place, std::invoke(std::forward<F>(f), std::move(**this)));
                }
            }
            return expected<U, E>(unexpect, std::move(error()));
        }
        /*
         * transform() method to apply a function to the value.
         *
         * @tparam F - The type of the function to apply.
         *
         * @param f - The function to apply.
         *
         * @return The result of the function if the `expected` object has a value, the error value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `T`.
         * - If the result of `F` is not `void`, `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform(F&& f) const&& 
            noexcept(
                std::is_nothrow_invocable_v<F, decltype(std::move(**this))> and (
                    std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>> or
                    std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>, std::invoke_result_t<F, decltype(std::move(**this))>>
                ) and
                std::is_nothrow_constructible_v<E, decltype(std::move(error()))>
            )
            requires strict_invocable_v<F, decltype(std::move(**this))> and (
                        std::is_void_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>> or 
                        strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>, std::invoke_result_t<F, decltype(std::move(**this))>>
                     ) and strict_constructible_v<E, decltype(std::move(error()))>
        {
            using U = std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(**this))>>;
            if (has_value()) [[likely]] {
                if constexpr (std::is_void_v<U>) {
                    std::invoke(std::forward<F>(f), std::move(**this));
                    return expected<U, E>();
                } else {
                    return expected<U, E>(in_place, std::invoke(std::forward<F>(f), std::move(**this)));
                }
            }
            return expected<U, E>(unexpect, std::move(error()));
        }
        
        /*
         * transform_error() method to apply a function to the error value.
         *
         * @tparam F - The type of the function to apply.
         *
         * @param f - The function to apply.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform_error(F&& f) & noexcept(std::is_nothrow_invocable_v<F, decltype(error())> and std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>, std::invoke_result_t<F, decltype(error())>> and
            std::is_nothrow_constructible_v<T, decltype((**this))>)
            requires strict_invocable_v<F, decltype(error())> and
                     strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>, std::invoke_result_t<F, decltype(error())>> and
                     strict_constructible_v<T, decltype((**this))>
        {
            using G = std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>;
            if (has_value()) [[likely]]
                return expected<T, G>(in_place, **this);
            return expected<T, G>(unexpect, std::invoke(std::forward<F>(f), error()));
        }
        /*
         * transform_error() method to apply a function to the error value.
         *
         * @tparam F - The type of the function to apply.
         *
         * @param f - The function to apply.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform_error(F&& f) const& noexcept(std::is_nothrow_invocable_v<F, decltype(error())> and std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>, std::invoke_result_t<F, decltype(error())>> and
            std::is_nothrow_constructible_v<T, decltype((**this))>)
            requires strict_invocable_v<F, decltype(error())> and
                     strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>, std::invoke_result_t<F, decltype(error())>> and
                     strict_constructible_v<T, decltype((**this))>
        {
            using G = std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>;
            if (has_value()) [[likely]]
                return expected<T, G>(in_place, **this);
            return expected<T, G>(unexpect, std::invoke(std::forward<F>(f), error()));
        }
        /*
         * transform_error() method to apply a function to the error value.
         *
         * @tparam F - The type of the function to apply.
         *
         * @param f - The function to apply.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform_error(F&& f) && noexcept(std::is_nothrow_invocable_v<F, decltype(std::move(error()))> and std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(error()))>>, std::invoke_result_t<F, decltype(std::move(error()))>> and
            std::is_nothrow_constructible_v<T, decltype(std::move(**this))>)
            requires strict_invocable_v<F, decltype(std::move(error()))> and
                     strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(error()))>>, std::invoke_result_t<F, decltype(std::move(error()))>> and
                     strict_constructible_v<T, decltype(std::move(**this))>
        {
            using G = std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>;
            if (has_value()) [[likely]]
                return expected<T, G>(in_place, std::move(**this));
            return expected<T, G>(unexpect, std::invoke(std::forward<F>(f), std::move(error())));
        }
        /*
         * transform_error() method to apply a function to the error value.
         *
         * @tparam F - The type of the function to apply.
         *
         * @param f - The function to apply.
         *
         * @return The result of the function if the `expected` object has an error value, the value otherwise.
         *
         * Enabled only if:
         * - `F` is (nothrow) invocable with `E`.
         * - `T` is (nothrow) constructible from the result of `F`.
         * - `E` is (nothrow) constructible from the error value.
         */
        template <typename F>
        constexpr auto transform_error(F&& f) const&& noexcept(std::is_nothrow_invocable_v<F, decltype(std::move(error()))> and std::is_nothrow_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(error()))>>, std::invoke_result_t<F, decltype(std::move(error()))>> and
            std::is_nothrow_constructible_v<T, decltype(std::move(**this))>)
            requires strict_invocable_v<F, decltype(std::move(error()))> and
                     strict_constructible_v<std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(error()))>>, std::invoke_result_t<F, decltype(std::move(error()))>> and
                     strict_constructible_v<T, decltype(std::move(**this))>
        {
            using G = std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>;
            if (has_value()) [[likely]]
                return expected<T, G>(in_place, std::move(**this));
            return expected<T, G>(unexpect, std::invoke(std::forward<F>(f), std::move(error())));
        }
    private:
        union {
            T _value; // The value stored in the `expected` object.
            E _error; // The error value stored in the `expected` object.
        };
        bool _has_value; // Flag to indicate if the `expected` object has a value.
    };

    template <typename E>
    class expected<void, E> {
    public:
        constexpr expected() noexcept
            : _dummy(), _has_value(true) {}

        template <typename... Args>
        constexpr expected(unexpect_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
            requires strict_constructible_v<E, Args...>
            : _error(std::forward<Args>(args)...), _has_value(false) {}

        constexpr ~expected() noexcept(std::is_nothrow_destructible_v<E>)
            requires strict_destructible_v<E>
        {
            if (not has_value()) [[unlikely]]
                std::destroy_at(std::addressof(_error));
        }

        constexpr explicit operator bool() const noexcept { return _has_value; }
        constexpr bool has_value() const noexcept { return _has_value; }

        constexpr const E& error() const& noexcept {
            assert(not has_value());
            return _error;
        }
        constexpr E& error() & noexcept {
            assert(not has_value());
            return _error;
        }
        constexpr const E&& error() const&& noexcept {
            assert(not has_value());
            return std::move(_error);
        }
        constexpr E&& error() && noexcept {
            assert(not has_value());
            return std::move(_error);
        }
    private:
        struct dummy_t { constexpr dummy_t() noexcept = default; };
        union {
            dummy_t _dummy{}; // Dummy value to avoid warning of uninitialized value.
            E _error;
        };
        bool _has_value;
    };
}