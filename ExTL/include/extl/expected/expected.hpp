#pragma once

#include <cassert>
#include <memory>
#include <extl/utility/type_traits.hpp>
#include <extl/expected/inplace.hpp>
#include <extl/expected/unexpected.hpp>

namespace extl {
    template <typename T, typename E>
    class expected;
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

        constexpr expected() noexcept(std::is_nothrow_default_constructible_v<T>)
            requires strict_default_constructible_v<T>
        : _value(), _has_value(true) {}

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
         
        template <typename U, typename G >
        constexpr explicit(not std::is_convertible_v<const U&, T> and not std::is_convertible_v<const G&, E>) expected(const expected<U, G>& other) noexcept(std::is_nothrow_constructible_v<T, const U&> and std::is_nothrow_constructible_v<E, const G&>)
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

        template <typename U, typename G>
        constexpr explicit(not std::is_convertible_v<U&&, T> and not std::is_convertible_v<G&&, E>) expected(expected<U, G>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&> and not std::is_nothrow_constructible_v<E, G&&>)
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

        template <typename U = T >
        constexpr explicit(not std::is_convertible_v<U, T>) expected(U&& v) noexcept(std::is_nothrow_constructible_v<T, U>)
            requires (not std::is_same_v<std::remove_cvref_t<U>, in_place_t>) and
                     (not std::is_same_v<expected<T, E>, std::remove_cvref_t<U>>) and
                     strict_constructible_v<T, U> and
                     (not is_unexpected<std::remove_cvref_t<U>>) and
                     ((not std::is_same_v<bool, std::remove_cv_t<T>>) or is_expected<std::remove_cvref_t<U>>)
            : _value(std::forward<U>(v)), _has_value(true) {}

        template <typename G>
        constexpr explicit(not std::is_convertible_v<const G&, E>) expected(const unexpected<G>& e) noexcept(std::is_nothrow_constructible_v<E, const G&>)
            requires strict_constructible_v<E, const G&>
        : _error(e.error()), _has_value(false) {}

        template <typename G>
        constexpr explicit(not std::is_convertible_v<G&&, E>) expected(unexpected<G>&& e) noexcept(std::is_nothrow_constructible_v<E, G&&>)
            requires strict_constructible_v<E, G&&>
        : _error(std::move(e.error())), _has_value(false) {}

        template <typename... Args>
        constexpr explicit expected(in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
            requires strict_constructible_v<T, Args...>
        : _value(std::forward<Args>(args)...), _has_value(true) {}

        template <typename... Args>
        constexpr explicit expected(unexpect_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
            requires strict_constructible_v<E, Args...>
            : _error(std::forward<Args>(args)...), _has_value(false) {}

        ~expected() noexcept(std::is_nothrow_destructible_v<T> and std::is_nothrow_destructible_v<E>)
            requires strict_destructible_v<T> and strict_destructible_v<E>
        {
            if (has_value()) [[likely]]
                std::destroy_at(std::addressof(_value));
            else
                std::destroy_at(std::addressof(_error));
        }

        constexpr const T* operator->() const noexcept {
            assert(has_value());
            return std::addressof(_value);
        }
        constexpr T* operator->() noexcept {
            assert(has_value());
            return std::addressof(_value);
        }

        constexpr const T& operator*() const& noexcept {
            assert(has_value());
            return _value;
        }
        constexpr T& operator*() & noexcept {
            assert(has_value());
            return _value;
        }
        constexpr const T&& operator*() const&& noexcept {
            assert(has_value());
            return std::move(_value);
        }
        constexpr T&& operator*() && noexcept {
            assert(has_value());
            return std::move(_value);
        }

        constexpr explicit operator bool() const noexcept { return _has_value; }
        constexpr bool has_value() const noexcept { return _has_value; }

        constexpr const T& value() const& noexcept {
            assert(has_value());
            return _value;
        }
        constexpr T& value() & noexcept {
            assert(has_value());
            return _value;
        }
        constexpr const T&& value() const&& noexcept {
            assert(has_value());
            return std::move(_value);
        }
        constexpr T&& value() && noexcept {
            assert(has_value());
            return std::move(_value);
        }

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

        template <typename U>
        constexpr T value_or(U&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<T> and std::is_nothrow_convertible_v<U, T>)
            requires strict_copy_constructible_v<T> and strict_convertible_v<U, T>
        {
            if (has_value()) [[likely]]
                return **this;
            return static_cast<T>(std::forward<U>(default_value));
        }
        template <typename U>
        constexpr T value_or(U&& default_value) && noexcept(std::is_nothrow_move_constructible_v<T> and std::is_nothrow_convertible_v<U, T>)
            requires strict_move_constructible_v<T> and strict_convertible_v<U, T>
        {
            if (has_value()) [[likely]]
                return std::move(**this);
            return static_cast<T>(std::forward<U>(default_value));
        }

        template <typename G = E>
        constexpr E error_or(G&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<E> and std::is_nothrow_convertible_v<G, E>)
            requires strict_copy_constructible_v<E> and strict_convertible_v<G, E>
        {
            if (has_value()) [[likely]]
                return static_cast<E>(std::forward<G>(default_value));
            return error();
        }
        template <typename G = E>
        constexpr E error_or(G&& default_value) && noexcept(std::is_nothrow_move_constructible_v<E> and std::is_nothrow_convertible_v<G, E>)
            requires strict_move_constructible_v<E> and strict_convertible_v<G, E>
        {
            if (has_value()) [[likely]]
                return static_cast<E>(std::forward<G>(default_value));
            return std::move(error());
        }

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
    private:
        union {
            T _value;
            E _error;
        };
        bool _has_value;
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
            dummy_t _dummy;
            E _error;
        };
        bool _has_value;
    };
}