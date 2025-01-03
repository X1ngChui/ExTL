#pragma once

#include <cassert>
#include <concepts>
#include <memory>
#include <type_traits>

#include <memory/in_place.hpp>

#define unreachable() assert(false)

namespace extl {
	// nullerr_t is used to construct a status object that does not contain an error
	struct nullerr_t {
		explicit constexpr nullerr_t(int) noexcept {}
	};
	inline constexpr nullerr_t nullerr{ 0 };

	// status_base is the base class for all status objects so that we can use concepts to check if a type is a status object
	struct status_base {};
	template <typename T>
	concept is_status = std::derived_from<T, status_base>;

	// status is a class that represents a potential error
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
		constexpr status(const status& other) noexcept 
			requires std::is_nothrow_copy_constructible_v<error_type>
			: _dummy{}, _has_error{ other._has_error }
		{
			if (_has_error)
				std::construct_at(&_error, *other);
		}

		// Move constructor
		// If `other` contains an error, initializes the contained error as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `std::move(*other)` and does not make other empty: a moved-from std::error still contains an error, but the error itself is moved from.
		// If `other` does not contain an error, constructs an object that does not contain an error.
		constexpr status(status&& other) noexcept
			requires std::is_nothrow_move_constructible_v<error_type>
			: _dummy{}, _has_error{ other._has_error }
		{
			if (_has_error)
				std::construct_at(&_error, std::move(*other));
		}
	
		// Converting copy constructor
		// If `other` does not contain an error, constructs an error object that does not contain an error. 
		// Otherwise, constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `*other`.
		// This constructor is explicit if and only if `std::is_convertible_v<const U&, E>` is false.
		template <typename U>
		constexpr explicit(!std::is_convertible_v<const U&, error_type>) status(const status<U>& other) noexcept
			requires std::is_nothrow_constructible_v<error_type, const U&> && (
					 std::is_same_v<error_type, bool> || (
						 !std::is_constructible_v<error_type, status<U>&> &&
						 !std::is_constructible_v<error_type, const status<U>&> &&
						 !std::is_constructible_v<error_type, status<U>&&> &&
						 !std::is_constructible_v<error_type, const status<U>&&> &&
						 !std::is_convertible_v<status<U>&, error_type> &&
						 !std::is_convertible_v<const status<U>&, error_type> &&
						 !std::is_convertible_v<status<U>&&, error_type> &&
						 !std::is_convertible_v<const status<U>&& , error_type>
						 )
				     )
			: _dummy{}, _has_error{ other._has_error }
		{
			if (_has_error)
				std::construct_at(&_error, *other);
		}

		// Converting move constructor
		// If `other` does not contain an error, constructs an error object that does not contain an error. 
		// Otherwise, constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `std::move(*other)`.
		// This constructor is explicit if and only if std::is_convertible_v<U&&, E> is false.
		template <typename U>
		constexpr explicit(!std::is_convertible_v<U&&, error_type>) status(status<U>&& other) noexcept
			requires std::is_nothrow_constructible_v<error_type, U&&> && (
						 std::is_same_v<error_type, bool> || (
							 !std::is_constructible_v<error_type, status<U>&> &&
							 !std::is_constructible_v<error_type, const status<U>&> &&
							 !std::is_constructible_v<error_type, status<U>&&> &&
							 !std::is_constructible_v<error_type, const status<U>&&> &&
							 !std::is_convertible_v<status<U>&, error_type> &&
							 !std::is_convertible_v<const status<U>&, error_type> &&
							 !std::is_convertible_v<status<U>&&, error_type> &&
							 !std::is_convertible_v<const status<U>&&, error_type>
						 )
				     )
			: _dummy{}, _has_error{ other._has_error }
		{
			if (_has_error)
				std::construct_at(&_error, std::move(*other));
		}

		// Constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T from the arguments `std::forward<Args>(args)...`.
		template <typename... Args>
		constexpr explicit status(in_place_t, Args&&... args) noexcept
			requires std::is_nothrow_constructible_v<error_type, Args...>
			: _dummy{}, _has_error{ true }
		{
			std::construct_at(&_error, std::forward<Args>(args)...);
		}

		// Constructs an error object that contains an error, initialized as if direct-initializing (but not direct-list-initializing) an object of type T with the expression `std::forward<U>(value)`.
		// This constructor is explicit if and only if `std::is_convertible_v<U&&, E>` is false.
		template <typename U = E>
		constexpr explicit(!std::is_convertible_v<U&&, E>) status(U&& value) noexcept 
			requires std::is_nothrow_constructible_v<error_type, U&&> && (
						 !std::is_same_v<std::remove_cvref_t<U>, nullerr_t> &&
						 !std::is_same_v<std::remove_cvref_t<U>, status<error_type>>
				     ) && (
						 !std::is_same_v<error_type, bool> ||
						 !is_status<std::is_same_v<std::remove_cvref_t<U>>>
				     )
			: _dummy{}, _has_error{ true }
		{
			std::construct_at(&_error, std::forward<U>(value));
		}

		// --- Destructor ---
		// Destroys the contained error if there is one.
		constexpr ~status() noexcept 
			requires std::is_nothrow_destructible_v<error_type>
		{
			if (_has_error) std::destroy_at(&_error);
		}

		// --- Observers ---
		constexpr bool ok() const noexcept { return !_has_error; }
		constexpr explicit operator bool() const noexcept { return ok(); }

		constexpr const error_type& error() const& noexcept { assert(!ok()); return _error; }
		constexpr error_type& error() & noexcept { assert(!ok()); return _error; }

		constexpr const error_type&& error() const&& noexcept { assert(!ok()); return std::move(_error); }
		constexpr error_type&& error() && noexcept { assert(!ok()); return std::move(_error); }

		constexpr const error_type* operator->() const noexcept { return &error(); }
		constexpr error_type* operator->() noexcept { return &error(); }
		
		constexpr const error_type& operator*() const& noexcept { return error(); }
		constexpr error_type& operator*() & noexcept { return error(); }
		
		constexpr const error_type&& operator*() const&& noexcept { return error(); }
		constexpr error_type&& operator*() && noexcept { return error(); }

	private:
		// dummy_t is used to avoid default-initialization of error_type
		struct dummy_t { constexpr dummy_t() = default; };

		union {
			dummy_t _dummy; // to avoid default-initialization of error_type
			error_type _error; // the actual error
		};

		bool _has_error; // whether the error is present

		template <typename U>
		friend class status;
	};

	struct always_ok_status {
		// --- Constructors ---
		constexpr always_ok_status() noexcept = default;
		constexpr always_ok_status(const always_ok_status&) noexcept = default;
		constexpr always_ok_status(always_ok_status&&) noexcept = default;
		constexpr always_ok_status& operator=(const always_ok_status&) noexcept = default;
		constexpr always_ok_status& operator=(always_ok_status&&) noexcept = default;

		// --- Observers ---
		constexpr bool ok() const noexcept { return true; }
		constexpr explicit operator bool() const noexcept { return ok(); }
	};
}