#pragma once

#include "stl_deque.h"
#include "stl_vector.h"
#include "stl_function.h"
#include "heap_algorithm.h"

namespace STL {

	template <class T, class Sequence = deque<T> >
	class queue {

		template <class _T, class _Sequence>
		friend bool operator==(const queue<_T, _Sequence>&, const queue<_T, _Sequence>&);

		template <class _T, class _Sequence>
		friend bool operator<(const queue<_T, _Sequence>&, const queue<_T, _Sequence>&);
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
	private:
		Sequence c;
	public:
		bool empty() const noexcept { return c.empty(); }
		size_type size() const noexcept { return c.size(); }
		reference front() noexcept { return c.front(); }
		const_reference front() const noexcept { return c.front(); }
		reference back() noexcept { return c.back(); }
		const_reference back() const noexcept { return c.back(); }
		void push(const value_type& x) { c.push_back(x); }
		void pop() { c.pop_front(); }
	};

	template <class T, class Sequence>
	bool operator==(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
		return x.c == y.c;
	}

	template <class T, class Sequence>
	bool operator<(const queue<T, Sequence>& x, const queue<T, Sequence>& y) {
		return x.c < y.c;
	}

	template <class T, class Sequence = vector<T>, class Compare = less<typename Sequence::value_type>>
	class priority_queue {
	public:
		using value_type = typename Sequence::value_type;
		using size_type = typename Sequence::size_type;
		using reference = typename Sequence::reference;
		using const_reference = typename Sequence::const_reference;
	private:
		Sequence c;
		Compare comp;
	public:
		priority_queue() : c() { }
		explicit priority_queue(const Compare& x) : c(), comp(x) { }

		template <class InputIterator>
		priority_queue(InputIterator first, InputIterator last, const Compare& x)
			: c(first, last) { make_heap(c.begin(), c.end(), comp); }
		template <class InputIterator>
		priority_queue(InputIterator first, InputIterator last)
			: c(first, last) { make_heap(c.begin(), c.end(), comp); }

		bool empty() const noexcept { return c.empty(); }
		size_type size() const noexcept { return c.size(); }
		const_reference top() const noexcept { return c.front(); }
		void push(const value_type& x) {
			__STL_TRY{
				c.push_bakc(x);
				push_heap(c.begin(), c.end(), comp);
			}
			__STL_UNWIND(c.clear());
		}
		void pop(const value_type& x) {
			__STL_TRY{
				pop_heap(c.begin(), c.end(), comp)
				c.pop_back(x);
			}
			__STL_UNWIND(c.clear());
		}
	};
}
