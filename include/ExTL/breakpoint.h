#pragma once
#include <functional>
#include <mutex>

#include "allocator.h"

namespace extl {
	void breakpoint() {
#if defined(_MSC_VER)
		__debugbreak();
#elif defined(__clang__) || defined(__GNUC__)
		__builtin_trap();
#else
		std::terminate();
#endif
	}
}