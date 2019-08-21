#pragma once

#include <new>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <iostream>

namespace simpleAlloc {

	template <class T>
	inline T* __allocate(ptrdiff_t size, T*) {
		std::set_new_handler(nullptr);
		T* tmp = static_cast<T*>(::operator new(static_cast<size_t>(size * sizeof(T))));
		if (tmp == nullptr) {
			std::cerr << "out of memory";
			exit(-1);
		}
		return tmp;
	}

	template <class T>
	inline void __deallocate(T* buffer) {
		::operator delete(buffer);
	}

	template <class T1, class T2>
	inline void __construct(T1* p, const T2& value) {
		new (p) T1(value);
	}

	template <class T>
	inline void __destory(T* p) {
		p->~T();
	}

	template <class T>
	class allocator {
	public:
		using value_type = T;
		using pointer = T *;
		using const_pointer = const T*;
		using reference = T &;
		using const_refernce = const T &;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		template <class U>
		struct rebind { using other = allocator<U>; };

		pointer allocate(size_type n, const void* hint = nullptr) {
			return __allocate(static_cast<difference_type>(n), nullptr);
		}
		void deallocate(pointer p, size_type n) {
			__deallocate(p);
		}
		void construct(pointer p, const T& value) {
			__construct(p, value);
		}
		void destroy(pointer p) {
			__destory(p);
		}

		pointer address(reference x) {
			return static_cast<pointer>(&x);
		}
		const_pointer const_address(reference x) {
			return static_cast<const_pointer>(&x);
		}
		size_type max_size() const {
			return size_type(UINT_MAX / sizeof(T));
		}
	};
}
