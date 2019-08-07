#pragma once

#include <cstddef>
#include "stl_iterator.h"

namespace STL {

	inline size_t __deque_buf_size(size_t n, size_t sz) {
		return n != 0 ? n : (sz < 521 ? size_t(512 / sz) : size_t(1));
	}

	template <class T, class Ref, class Ptr, size_t Bufsiz>
	struct __deque_iterator {
		using iterator = __deque_iterator<T, T&, T*, Bufsiz>;
		using const_iterator = __deque_iterator<T, const T&, const T*, Bufsiz>;
		static size_t buffer_size() { return __deque_buf_size(Bufsiz, sizeof(T)); }

		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using map_pointer = T * *;

		using self = __deque_iterator;

		T* cur;
		T* first;
		T* last;
		map_pointer node;

		__deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) { }

		__deque_iterator(pointer x, map_pointer y) : cur(x), first(*y), last(*y + buffer_size()), node(y) { }

		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = first + static_cast<difference_type>(buffer_size());
		}

		reference operator*() const { return *cur; }
		pointer operator->() const { return &(operator*()); }
		difference_type operator-(const self& x) const {
			return static_cast<difference_type>(buffer_size())* (node - x.node - 1) +
				(cur - first) + (x.last - x.cur);
		}

		self& operator++() {
			++cur;
			if (cur == last) {
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}
		self& operator--() {
			--cur;
			if (cur == first) {
				set_node(node - 1);
				cur = first;
			}
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--* this;
			return tmp;
		}

		self& operator+=(difference_type n) {
			difference_type offset = n + (cur - first);
			if (offset >= 0 && offset < static_cast<difference_type>(buffer_size()))
				cur += n;
			else {
				difference_type node_offset = offset > 0 ? offset / static_cast<difference_type>(buffer_size())
					: -static_cast<difference_type>(-offset - 1) / buffer_size() - 1;
				set_node(node + node_offset);
				cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size()))
			}
			return *this;
		}

		self operator+(difference_type n) const {
			self tmp = *this;
			return tmp += n;
		}

		self operator-(difference_type n) const {
			self tmp = *this;
			return tmp -= n;
		}

		reference operator[](difference_type n) const { return *(*this + n); }

		bool operator==(const self& x) const { return cur == x.cur; }
		bool operator!=(const self& x) const { return !(*this == x); }
		bool operator<(const self& x) const { return (node == x.node) ? (cur < x.cur) : (node < x.node); }
	};
}
