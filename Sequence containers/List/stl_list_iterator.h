#pragma once

#include <cstddef>
#include "stl_iterator.h"
#include "stl_list_node.h"

namespace STL {

	template <class T, class Ref, class Ptr>
	struct __list_iterator {
		using iterator = __list_iterator<T, T&, T*>;
		using self = __list_iterator<T, Ref, Ptr>;
		 
		using iterator_category = bidirectional_iterator_tag;
		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using link_type = __list_node<T>*;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		link_type node;

		__list_iterator(link_type x) : node(x) { }
		__list_iterator() { }
		explicit __list_iterator(const iterator& x) : node(x.node) { }

		bool operator==(const self& x) const noexcept { return node == x.node; }
		bool operator!=(const self& x) const noexcept { return node != x.node; }
		reference operator*() const { return (*node).data; }

		pointer operator->() const { return &(operator*()); }

		self& operator++() {
			node = static_cast<link_type>((*node).next);
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}

		self& operator--() {
			node = static_cast<link_type>((*node).next);
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--*this;
			return tmp;
		}
	};
}
