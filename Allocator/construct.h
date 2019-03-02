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
		_destroy(first, last, value_type(first));
	}

	template<class ForwardIterator>
	inline void _destroy(ForwardIterator first, ForwardIterator last) {
		using is_POD_type = typename _type_traits<ForwardIterator>::is_POD_type;
		_destroy_aux(first, last, is_POD_type());
	}

	template<class ForwardIterator>
	inline void _destroy(ForwardIterator first, ForwardIterator last, _false_type) {
		for (; first != last; ++first)
			destory(&*first);
	}

	template<class ForwardIterator>
	inline void _destroy_aux(ForwardIterator first, ForwardIterator last, _true_type) {}

	inline void destroy(char*, char*) {}
	inline void destroy(wchar_t*, wchar_t*) {}
}
