#pragma once

namespace extl {
	struct in_place_t {
		explicit constexpr in_place_t() noexcept = default;
	};
	inline constexpr in_place_t in_place{};
}