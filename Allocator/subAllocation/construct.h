#pragma once

#include <new>
#include "typeTraits.h"

namespace STL {

	template <class T1, class T2>
	inline void construct(T1* p, const T2& value) {
		new(p) T1(value);
	}

	template<class T>
	inline void destroy(T* pointer) {
		pointer->~T();
	}

	template<class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		__destroy(first, last, value_type(first));
	}

	template<class ForwardIterator>
	inline void __destroy(ForwardIterator first, ForwardIterator last) {
		using is_POD = typename __type_traits<ForwardIterator>::is_POD_type;
		__destroy_aux(first, last, is_POD());
	}

	template<class ForwardIterator>
	inline void __destroy(ForwardIterator first, ForwardIterator last, __false_type) {
		for (; first != last; ++first)
			destory(&*first);
	}

	template<class ForwardIterator>
	inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

	inline void destroy(char*, char*) {}
	inline void destroy(wchar_t*, wchar_t*) {}
}
