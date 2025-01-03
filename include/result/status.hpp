#pragma once

#include <cassert>
#include <concepts>
#include <initializer_list>
#include <type_traits>

#include <memory/lifecycle.hpp>

#define unreachable() assert(false)

namespace extl {
	struct in_place_t;

	struct nullerr_t {
		explicit constexpr nullerr_t(int) noexcept {}
	};
	inline constexpr nullerr_t nullerr{ 0 };

	struct status_base {};
	template <typename T>
	concept is_status = std::derived_from<T, status_base>;

	template <typename E>
	class status : public status_base {
	public:
		using error_type = E;

		// --- Constructors ---
		// Constructs an object that does not contain a error.
		constexpr status() noexcept : _dummy{}, _has_error{ false } {}
		constexpr status(nullerr_t) noexcept : _dummy{}, _has_error{ false } {}

		// Copy constructor
		// If `other` contains an error, initializes the contained error as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `*other`.
		// If `other` does not contain an error, constructs an object that does not contain an error.
		constexpr status(const status& other)
			noexcept(std::is_nothrow_constructible_v<error_type>);

		// Move constructor
		// If `other` contains an error, initializes the contained error as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `std::move(*other)` and does not make other empty: a moved-from std::error still contains an error, but the error itself is moved from.
		// If `other` does not contain an error, constructs an object that does not contain an error.
		constexpr status(status&& other);

		// Converting copy constructor
		// If `other` does not contain an error, constructs an error object that does not contain an error. 
		// Otherwise, constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `*other`.
		template <typename U>
		constexpr status(const status<U>& other);

		// Converting move constructor
		// If `other` does not contain an error, constructs an error object that does not contain an error. 
		// Otherwise, constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `std::move(*other)`.
		template <typename U>
		constexpr status(status<U>&& other);

		// Constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T from the arguments `std::forward<Args>(args)...`.
		template <typename... Args>
		constexpr explicit status(in_place_t, Args&&... args);

		// Constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T from the arguments `ilist`, `std::forward<Args>(args)...`.
		template <typename U, typename... Args>
		constexpr explicit status(in_place_t, std::initializer_list<U> ilist, Args&&... args);

		// Constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `std::forward<U>(other)`.
		template <typename U = E>
		constexpr status(U&& other);

		// --- Destructor ---
		// Destroys the contained error if there is one.
		constexpr ~status() noexcept { if (_has_error) destroy_at(&_error); }

		// --- Observers ---
		constexpr bool ok() const noexcept { return !_has_error; }
		constexpr explicit operator bool() const noexcept { return ok(); }
		constexpr error_type& error() noexcept { assert(!ok()); return _error; }
		constexpr const error_type& error() const noexcept { assert(!ok()); return _error; }

	private:
		// dummy_t is used to avoid default-initialization of error_type
		struct dummy_t { constexpr dummy_t() = default; };

		union {
			dummy_t _dummy; // to avoid default-initialization of error_type
			error_type _error; // the actual error
		};

		bool _has_error; // whether the error is present
	};

	struct always_success_status {
		// --- Constructors ---
		constexpr always_success_status() noexcept = default;
		constexpr always_success_status(const always_success_status&) noexcept = default;
		constexpr always_success_status(always_success_status&&) noexcept = default;
		constexpr always_success_status& operator=(const always_success_status&) noexcept = default;
		constexpr always_success_status& operator=(always_success_status&&) noexcept = default;

		// --- Observers ---
		constexpr bool ok() const noexcept { return true; }
		constexpr explicit operator bool() const noexcept { return ok(); }
		constexpr void error() const noexcept { unreachable(); }
	};
}