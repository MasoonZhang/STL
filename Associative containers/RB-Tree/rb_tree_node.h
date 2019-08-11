#pragma once

namespace STL {
	using __rb_tree_color_type = bool;
	const __rb_tree_color_type __rb_tree_red = false;
	const __rb_tree_color_type __rb_tree_black = true;

	struct __rb_tree_node_base {
		using color_type = __rb_tree_color_type;
		using base_ptr = __rb_tree_node_base *;

		color_type color;
		base_ptr parent;
		base_ptr left;
		base_ptr right;

		static base_ptr minimum(base_ptr x) {
			while (x->left != nullptr) x = x->left;
			return x;
		}

		static base_ptr maximum(base_ptr x) {
			while (x->right != nullptr) x = x->right;
			return x;
		}
	};

	template <class Value>
	struct __rb_tree_node : public __rb_tree_node_base {
		using link_type = __rb_tree_node<Value>*;
		Value value_field;
	};
}
