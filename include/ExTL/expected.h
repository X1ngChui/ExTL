#pragma once
#include <utility>

#include "breakpoint.h"

namespace extl {
	template <typename H, typename E>
	concept ExpectedNoValueHandler = requires(H handler, E& error) {
		{ handler(error) } noexcept;
	};

	template <typename T, typename E>
	struct default_expected_no_value_handler {
		void operator()([[maybe_unused]] E& _error) noexcept {
			breakpoint();
		}
	};

	template <typename E>
	class unexpected {
	public:
		explicit unexpected(E&& _error) noexcept(std::is_nothrow_constructible_v<E, E&&>)
			: _error(std::forward<E>(_error)) {}

		E& error() noexcept {
			return _error;
		}

		const E& error() const noexcept {
			return _error;
		}
	private:
		E _error;
	};

	template <typename T, typename E, typename H = default_expected_no_value_handler<T, E>>
		requires ExpectedNoValueHandler<H, E>
	class expected : private H {
	public:
		explicit expected(T&& value) noexcept(std::is_nothrow_constructible_v<T, T&&>) 
			: _has_value(true) {
			new (&this->_value) T(std::forward<T>(value));
		}

		explicit expected(unexpected<E>&& error) noexcept(std::is_nothrow_constructible_v<E, E&&>)
			: _has_value(false) {
			new (&this->_error) E(std::move(error.error()));
		}

		expected(const expected&) = delete;
		expected& operator=(const expected&) = delete;

		expected(expected&& other) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<E>)
			: _has_value(other.has_value()) {
			if (_has_value) new (&_value) T(std::move(other._value));
			else new (&_error) E(std::move(other._error));
		}

		expected& operator=(expected&& other) noexcept(std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_move_assignable_v<E>) {
			if (this != &other) {
				_has_value = other._has_value;
				if (_has_value) new (&_value) T(std::move(other._value));
				else new (&_error) E(std::move(other._error));
			}
			return *this;
		}

		~expected() noexcept(std::is_nothrow_destructible_v<T> && std::is_nothrow_destructible_v<E>) {
			if (has_value()) [[likely]] _value.~T();
			else _error.~E();
		};

		bool has_value() const noexcept {
			return _has_value;
		};

		explicit operator bool() const noexcept {
			return has_value();
		};

		// Get the _value of the expected object
		// If the object has no value, the error handler is called
		T& value() noexcept {
			if (!has_value()) [[unlikely]] static_cast<H*>(this)->operator()(_error); 
			return _value;
		};

		const T& value() const noexcept {
			if (!has_value()) [[unlikely]] static_cast<H*>(this)->operator()(_error);
			return _value;
		};

		// Get the _error of the expected object
		// If the object has a value, the error handler is called
		E& error() noexcept {
			if (has_value()) [[unlikely]] static_cast<H*>(this)->operator()(_error);
			return _error;
		}

		const E& error() const noexcept {
			if (has_value()) [[unlikely]] static_cast<H*>(this)->operator()(_error);
			return _error;
		}
	private:
		bool _has_value;
		union {
			T _value;
			E _error;
		};
	};
}