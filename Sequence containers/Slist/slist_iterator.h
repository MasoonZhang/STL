#pragma once

#include <cstddef>
#include "slist_node.h"
#include "stl_iterator.h"

namespace STL {

	struct __slist_iterator_base {
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using iterator_category = forward_iterator_tag;

		__slist_node_base* node;

		__slist_iterator_base(__slist_node_base* x) : node(x) { }

		void incr() { node = node->next; }

		bool operator==(const __slist_iterator_base& x) const {
			return node == x.node;
		}

		bool operator!=(const __slist_iterator_base& x) const {
			return node != x.node;
		}
	};

	template <class T, class Ref, class Ptr>
	struct __slist_iterator : public __slist_iterator_base {
		using iterator = __slist_iterator_base<T, T&, T*>;
		using const_iterator = __slist_iterator_base<T, const T&, const T*>;
		using self = __slist_iterator_base<T, Ref, Ptr>;

		using value_type = T;
		using pointer = Ptr;
		using reference = Ref;
		using list_node = __slist_node<T>;

		__slist_iterator(list_node* x) : __slist_iterator_base(x) { }
		__slist_iterator() : __slist_iterator_base(nullptr) { }
		__slist_iterator(const iterator& x) : __slist_iterator_base(x.node) { }

		reference operator*() const { return ((list_node*)node)->data; }
		reference operator->() const { return &(operator*()); }

		self& operator++() {
			incr();
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			incr();
			return tmp;
		}
	};
}
