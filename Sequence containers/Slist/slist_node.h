#pragma once

namespace STL {

	struct __slist_node_base {
		__slist_node_base* next;
	};

	template <class T>
	struct __slist_node : public __slist_node_base {
		T data;
	};

	inline __slist_node_base* __slist_make_link(__slist_node_base* prev_node, __slist_node_base* new_node){
		new_node->next = prev_node->next;
		prev_node->next = new_node;
		return new_node;
	}

	inline size_t __slist_size(__slist_node_base* node) {
		size_t result = 0;
		for (; node != 0; node = node->next)
			++result;
		return result;
	}
}
