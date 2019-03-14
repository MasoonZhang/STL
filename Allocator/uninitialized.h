#pragma once

#include <string>
#include "construct.h"
#include "typeTraits.h"
#include "stl_iterator.h"
#include "stl_algobase.h"

namespace STL {

	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
		using isPODtype = typename _type_traits<value_type_t<ForwardIterator> >::is_POD_type;
		return _uninitialized_fill_n_aux(first, n, x, isPODtype());
	}

	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type) {
		return STL::fill_n(first, n, x);
	}

	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type) {
		ForwardIterator cur = first;
		for (; n > 0; --n, ++cur)
			construct(&*cur, x);
		return cur;
	}

	template <class InputIterator, class ForwardIterator>
	inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
		using isPODtype = typename _type_traits<value_type_t<InputIterator> >::is_POD_type;
		return _uninitialized_copy_aux(first, last, result, isPODtype());
	}

	template <class InputIterator, class ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _true_type) {
		return STL::copy(first, last, result);
	}

	template <class InputIterator, class ForwardIterator>
	inline ForwardIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, _false_type) {
		ForwardIterator cur = result;
		for (; first != last; ++first, ++cur)
			construct(&*cur, *first);
		return cur;
	}

	inline char* uninitialized_copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	template <class ForwardIterator, class T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
		using isPODtype = typename _type_traits<value_type_t<ForwardIterator> >::is_POD_type;
		_uninitialized_fill_aux(first, last, x, isPODtype());
	}

	template <class ForwardIterator, class T>
	inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, _true_type) {
		STL::fill(first, last, x);
	}

	template <class ForwardIterator, class T>
	inline void _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, _false_type) {
		ForwardIterator cur;
		for (; cur != last; ++cur)
			construct(&*cur, x);
	}
}
