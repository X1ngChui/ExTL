#pragma once

#include <concepts>
#include <type_traits>

#include <result/result.hpp>
#include <result/status.hpp>

namespace extl {
	struct in_place_t {
		explicit constexpr in_place_t() noexcept = default;
	};
	inline constexpr in_place_t in_place{};

	template <typename T>
	class storage {
	public:
		using value_type = T;

		constexpr storage() noexcept : _dummy{} {};
		constexpr ~storage() noexcept { destroy_at(&_value); }
		constexpr storage(const storage&) = delete;
		constexpr storage(storage&&) = delete;
		constexpr storage& operator=(const storage&) = delete;
		constexpr storage& operator=(storage&&) = delete;

		constexpr operator T* () noexcept { return reinterpret_cast<T*>(&_value); }
		constexpr explicit operator T& () noexcept { return *reinterpret_cast<T*>(&_value); }

	private:
		// dummy_t is used to avoid default-initialization of value_type
		struct dummy_t { constexpr dummy_t() = default; };

		union {
			dummy_t _dummy; // to avoid default-initialization of value_type
			value_type _value; // the actual value
		};
	};

	template <typename T, typename... Args>
	concept has_create = requires(Args&&... args) {
		{ T::create(std::forward<Args>(args)...) } -> is_result;
	};

	template <typename T, typename... Args>
	concept has_in_place_create = requires(storage<T>&storage, Args&&... args) {
		{ T::create(storage, std::forward<Args>(args)...) } -> is_status;
	};

	template <typename T>
	concept has_copy = requires(const T& value) {
		{ T::copy(value) } -> is_result;
	};

	template <typename T>
	concept has_in_place_copy = requires(storage<T>& storage, const T& value) {
		{ T::copy(storage, value) } -> is_status;
	};

	template <typename T, typename... Args>
	[[nodiscard]] static constexpr auto construct_at(storage<T>& storage, Args&&... args) noexcept
		requires has_in_place_create<T, Args...>
	{
		return T::create(storage, std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
		requires (!has_in_place_create<T, Args...>) && std::is_constructible_v<T, Args...>
	[[nodiscard]] static constexpr auto construct_at(storage<T>& storage, Args&&... args) noexcept
	{
		new(storage) T(std::forward<Args>(args)...);
		return always_success_status{};
	}

	template <typename T>
	[[nodiscard]] static constexpr auto copy_at(storage<T>& storage, const T& value) noexcept
		requires has_in_place_copy<T>
	{
		return T::copy(storage, value);
	}

	template <typename T>
		requires (!has_in_place_copy<T>) && std::is_copy_constructible_v<T>
	[[nodiscard]] static constexpr auto copy_at(storage<T>& storage, const T& value) noexcept
	{
		new(storage) T(value);
		return always_success_status{};
	}

	template <typename T>
	static constexpr void destroy_at(T* ptr) noexcept
	{
		ptr->~T();
	}
}