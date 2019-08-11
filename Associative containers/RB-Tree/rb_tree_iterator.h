#pragma once

#include <cstddef>
#include "rb_tree_node.h"
#include "stl_iterator.h"

namespace STL {
	struct __rb_tree_base_iterator {
		using base_ptr = __rb_tree_node_base::base_ptr;
		using iterator_category = bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;

		base_ptr node;

		void increment() {
			if (node->right != nullptr) {
				node = node->right;
				while (node->left != nullptr)
					node = node->left;
			}
			else {
				base_ptr y = node->parent;
				while (node == y->right) {
					node = y;
					y = y->parent;
				}
				if (node->right != y)
					node = y;
			}
		}

		void decrement() {
			if (node->color == __rb_tree_red && node->parent->parent == node)
				node = node->right;
			else if (node->left != nullptr) {
				base_ptr y;
				y = node->left;
				while (y->right != nullptr)
					y = y->right;
				node = y;
			}
			else {
				base_ptr y = node->parent;
				while (node == y->left) {
					node = y;
					y = y->parent;
				}
				node = y;
			}
		}
	};

	template <class T, class Ref, class Ptr>
	struct __rb_tree_iterator : public __rb_tree_base_iterator {
		using value_type = T;
		using reference = Ref;
		using pointer = Ptr;
		using iterator = __rb_tree_iterator<T, T&, T*>;
		using const_iterator = __rb_tree_iterator<T, const T&, const T*>;
		using self = __rb_tree_iterator<Value, Ref, Ptr>;
		using link_type = __rb_tree_node<Value>*;

		__rb_tree_iterator() { }
		__rb_tree_iterator(link_type x) { node = x; }
		__rb_tree_iterator(const iterator& it) { node = it.node; }

		reference operator*() const { return reinterpret_cast<link_type>(node)->value_field; }
		#ifndef __SGI_STL_NO_ARROW_OPERATOR
		pointer operator->() const { return &(operator*()); }
		#endif /* __SGI_STL_NO_ARROW_OPERATOR */

		self& operator++() { increment(); return *this; }
		self operator++(int) {
			self tmp = *this;
			increment();
			return tmp;
		}

		self& operator--() { decrement(); return *this; }
		self operator--(int) {
			self tmp = *this;
			decrement();
			return temp;
		}
	};
}
