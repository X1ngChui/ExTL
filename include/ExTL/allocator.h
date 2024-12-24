#pragma once

namespace extl {
	// Concept to check if type A is a noexcept allocator of type T
	template <typename A, typename T>
	concept Allocator = requires(A alloc, T* ptr, size_t n) {
		// The allocator must have the following functions

		// Allocate n objects of type T, returns nullptr if allocation fails
		{ alloc.allocate(n) } -> std::same_as<T*>; 
		{ alloc.allocate(n) } noexcept;
		
		// Deallocate n objects of type T, does nothing if ptr is nullptr
		{ alloc.deallocate(ptr, n) } -> std::same_as<void>;
		{ alloc.deallocate(ptr, n) } noexcept;
	};

	// Default allocator implementation that uses the global operator new and delete
	template <typename T>
	struct default_allocator {
		// Allocate n objects of type T
		// Returns nullptr if allocation fails
		T* allocate(size_t n) noexcept {
			return static_cast<T*>(::operator new(n * sizeof(T)), std::nothrow);
		}

		// Deallocate n objects of type T
		// Does nothing if ptr is nullptr
		void deallocate(T* ptr, size_t n) noexcept {
			::operator delete(ptr, n * sizeof(T), std::nothrow);
		}
	};
}