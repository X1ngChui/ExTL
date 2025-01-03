#pragma once

namespace extl {
	struct result_base {};
	template <typename T>
	concept is_result = std::derived_from<T, result_base>;

	template <typename T, typename E>
	struct result {
	};

	template <typename E>
	struct result<void, E> {
	};
}