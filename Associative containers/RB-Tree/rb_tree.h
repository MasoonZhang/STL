#pragma once

#include "allocator.h"
#include "stl_algobase.h"
#include "stl_function.h"
#include "rb_tree_iterator.h"

namespace STL {

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = simpleAlloc<Value> >
		class rb_tree {

		private:
			using void_pointer = void*;
			using base_ptr = __rb_tree_node_base *;
			using rb_tree_node = __rb_tree_node<Value>;
			using rb_tree_node_allocator = simpleAlloc<rb_tree_node>;
			using color_type = __rb_tree_color_type;

		public:
			using key_type = Key;
			using value_type = Value;
			using pointer = value_type *;
			using const_pointer = const value_type*;
			using reference = value_type &;
			using const_reference = const value_type &;
			using link_type = rb_tree_node *;
			using size_type = size_t;
			using difference_type = ptrdiff_t;

		private:
			link_type get_node() { return rb_tree_node_allocator::allocate(); }
			void put_node(link_type) { rb_tree_node_allocator::deallocate(p); }

			link_type create_node(const value_type& x) {
				link_type tmp = get_node();
				__STL_TRY{
					construct(&tmp->value_field, x);
				}
				__STL_UNWIND(put_node(tmp));
				return tmp;
			}

			link_type clone_node(link_type x) {
				link_type tmp = create_node(x->value_field);
				tmp->color = x->color;
				tmp->left = nullptr;
				tmp->right = nullptr;
				return tmp;
			}

			void destroy_node(link_type p) {
				destroy(&p->value_field);
				put_node(p);
			}

		private: 
			size_type node_count;
			link_type header;
			Compare key_compare;

		private:
			link_type& root() const noexcept { return reinterpret_cast<link_type&>(header->parent); }
			link_type& leftmost() const noexcept { return reinterpret_cast<link_type&>(header->left); }
			link_type& rightmost() const noexcept { return reinterpret_cast<link_type&>(header->right); }

			static link_type& left(link_type x) {
				return reinterpret_cast<link_type&>(x->left);
			}
			static link_type& right(link_type x) {
				return reinterpret_cast<link_type&>(x->right);
			}
			static link_type& parent(link_type x) {
				return reinterpret_cast<link_type&>(x->parent);
			}
			static reference& value(link_type x) {
				return x->value_field;
			}
			static const Key& key(link_type x) {
				return KeyOfValue()(value(x));
			}
			static color_type& color(link_type x) {
				return reinterpret_cast<color_type&>(x->color);
			}

			static link_type& left(base_ptr x) {
				return reinterpret_cast<link_type&>(x->left);
			}
			static link_type& right(base_ptr x) {
				return reinterpret_cast<link_type&>(x->right);
			}
			static link_type& parent(base_ptr x){
				return reinterpret_cast<link_type&>(x->parent);
			}
			static reference& value(base_ptr x) {
				return reinterpret_cast<link_type>(x)->value_field;
			}
			static const Key& key(base_ptr x) {
				return KeyOfValue()(value(reinterpret_cast<link_type>(x)));
			}
			static color_type& color(base_ptr x) {
				return reinterpret_cast<color_type&>(reinterpret_cast<link_type>(x)->color);
			}

			static link_type minimum(link_type x) {
				return reinterpret_cast<link_type>(__rb_tree_node_base::minimum(x));
			}
			static link_type maximum(link_type x) {
				return reinterpret_cast<link_type>(__rb_tree_node_base::maximum(x));
			}

		public:
			using iterator = __rb_tree_iterator<value_type, reference, pointer>;

		private:
			iterator __insert(base_ptr x, base_ptr y, const value_type& v);
			void init() {
				header = get_node();
				color(header) = __rb_tree_color_red;

				root() = nullptr;
				leftmost() = header;
				rightmost() = header;
			}

		private:
			void __rb_tree_rotate_left(base_ptr x, base_ptr& root);
			void __rb_tree_rotate_right(base_ptr x, base_ptr& root);
			void __rb_tree_rebalance(base_ptr x, base_ptr& root);

		public:
			rb_tree(const Compare& comp = Compare())
				: node_count(0), key_compare(comp) { init(); }
			~rb_tree() {
				clear();
				put_node(header);
			}
			rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
				if (this != &x) {
					clear();
					node_count = 0;
					key_compare = x.key_compare;
					if (!x.root()) {
						root() = nullptr;
						leftmost() = header;
						rightmost() = header;
					}
					else {
						root() = copy(x.root(), header);
						leftmost() = minimum(root());
						rightmost() = maximum(root());
						node_count = x.node_count;
					}
				}
				return *this;
			}

		public:
			Compare key_comp() const { return key_compare; }
			iterator begin() noexcept { return leftmost(); }
			iterator end() noexcept { return header; }
			bool empty() const noexcept { return node_count == 0; }
			size_type size() const noexcept { return node_count; }
			size_type max_size() const noexcept { return size_type(-1); }

		public:
			pair<iterator, bool> insert_unique(const value_type& x) {
				link_type y = header;
				link_type x = root();
				bool comp = true;
				while (x != nullptr) {
					y = x;
					comp = key_compare(KeyOfValue()(v), key(x));
					x = comp ? left(x) : right(x);
				}

				iterator j(y);
				if (comp)
					if (j == begin())
						return pair<iterator, bool>(insert_aux(x, y, v), true);
					else
						--j;
				if (key_compare(key(j.node), KeyOfValue()(v)))
					return pair<iterator, bool>(insert_aux(x, y, v), true);
				return pair<iterator, bool>(j, false);
			}
			iterator insert_equal(const value_type& x) {
				link_type y = header;
				link_type x = root();
				while (x != nullptr) {
					y = x;
					x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
				}
				return __insert(x, y, v);
			}

		private:
			void erase_aux(link_type) noexcept;

		public:
			void clear() noexcept;

		public:
			iterator find(const key_type&) noexcept;
	};

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const value_type& v) {
		link_type x = reinterpret_cast<link_type>(x_);
		link_type y = reinterpret_cast<link_type>(y_);
		link_type z;

		if (y == header || x || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);
			left(y) = z;
			if (y == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost()) {
				leftmost() = z;
			}
		}
		else {
			z = create_node(v);
			right(y) = z;
			if (y == rightmost())
				rightmost() = z;
		}
		parent(z) = y;
		left(z) = nullptr;
		right(z) = nullptr;

		__rb_tree_rebalance(z, header->parent);
		++node_count;
		return iterator(z);
	}


	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_tree_rotate_left(base_ptr x, base_ptr& root) {
		base_ptr y = x->right;
		x->right = y->left;
		if (y->left != nullptr)
			y->left->parent = x;
		y->parent = x->parent;
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare,
		Alloc>::__rb_tree_rotate_right(base_ptr x, base_ptr& root) {
		base_ptr y = x->left;
		x->left = y->right;
		if (y->right != nullptr)
			y->right->parent = x;
		y->parent = x->parent;
		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->right = y;
		else
			x->parent->left = y;
		y->right = x;
		x->parent = y;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_tree_rebalance(base_ptr x, base_ptr& root) {
		x->color = __rb_tree_red;
		while (x != root && x->parent->color == __rb_tree_red) {
			if (x->parent == x->parent->parent->left) {
				base_ptr y = x->parent->parent->right;
				if (y && y->color == __rb_tree_red) {
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {
					if (x == x->parent->right) {
						x = x->parent;
						__rb_tree_rotate_left(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else { 
				base_ptr y = x->parent->parent->left;
				if (y && y->color == __rb_tree_red) {
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {
					if (x == x->parent->left) {
						x = x->parent;
						__rb_tree_rotate_right(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}
		root->color = __rb_tree_black;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase_aux(link_type x) noexcept {
		while (x) {
			erase_aux(right(x));
			link_type y = left(x);
			destroy_node(x);
			x = y;
		}
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::clear() noexcept {
		if (node_count) {
			erase_aux(root());
			leftmost() = nheader;
			root() = nullptr;
			rightmost() = header;
			node_count = 0;
		}
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const key_type& k) noexcept {
		link_type y = header;
		link_type x = root();
		while (x != nullptr)
			if (!key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);
		iterator j = iterator(y);
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;

	}
}
