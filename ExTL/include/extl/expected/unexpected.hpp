#pragma once

#include <extl/utility/type_traits.hpp>
#include <extl/expected/inplace.hpp>

namespace extl {
    template <typename E>
    class unexpected;
    template <typename T>
    concept is_unexpected = is_specialization_of_v<T, unexpected>;

    /**
     * The `unexpected` class represents an error state in an `expected`-like object.
     * It holds an error value of type `E`.
     *
     * @tparam E - The type of the error value stored in the `unexpected` object.
     */
    template <typename E>
    class unexpected {
    public:
        /**
         * Default copy constructor.
         * Enabled only if `E` is nothrow copy constructible.
         */
        constexpr unexpected(const unexpected&) noexcept(std::is_nothrow_copy_constructible_v<E>)
            requires strict_copy_constructible_v<E> = default;

        /**
         * Default move constructor.
         * Enabled only if `E` is nothrow move constructible.
         */
        constexpr unexpected(unexpected&&) noexcept(std::is_nothrow_move_constructible_v<E>)
            requires strict_move_constructible_v<E> = default;

        /**
         * Constructor accepting an error value of type `Err`.
         *
         * Enabled if:
         * 1. `Err` is not the same as `unexpected` or `in_place_t` (to avoid ambiguities).
         * 2. `E` is nothrow constructible from `Err`.
         *
         * @param e - The error value used to construct the `unexpected` object.
         */
        template <typename Err = E>
        constexpr explicit unexpected(Err&& e) noexcept(std::is_nothrow_constructible_v<E, Err>)
            requires (not std::is_same_v<std::remove_cvref_t<Err>, unexpected>) and
                     (not std::is_same_v<std::remove_cvref_t<Err>, in_place_t>) and
                     strict_constructible_v<E, Err>
            : _error(std::forward<Err>(e)) {}

        /**
         * Constructor for in-place construction of the error value.
         *
         * Accepts a variable number of arguments to construct `E` directly.
         * Enabled if `E` is constructible from the provided arguments.
         *
         * @param args - The arguments used to construct the error value in place.
         */
        template <typename... Args>
        constexpr explicit unexpected(in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
            requires strict_constructible_v<E, Args...>
        : _error(std::forward<Args>(args)...) {}

        /**
         * Accessor for the error value (const lvalue reference).
         *
         * @return A const reference to the stored error value.
         */
        constexpr const E& error() const& noexcept { return _error; }

        /**
         * Accessor for the error value (mutable lvalue reference).
         *
         * @return A mutable reference to the stored error value.
         */
        constexpr E& error() & noexcept { return _error; }

        /**
         * Accessor for the error value (const rvalue reference).
         *
         * @return A const rvalue reference to the stored error value.
         */
        constexpr const E&& error() const&& noexcept { return std::move(_error); }

        /**
         * Accessor for the error value (mutable rvalue reference).
         *
         * @return A mutable rvalue reference to the stored error value.
         */
        constexpr E&& error() && noexcept { return std::move(_error); }
    private:
        E _error; // The stored error value of type `E`.
    };

    /**
     * Deduction guide for `unexpected`.
     *
     * This allows `unexpected` objects to be constructed without explicitly specifying the template parameter `E`.
     */
    template <typename E>
    unexpected(E) -> unexpected<E>;
}
