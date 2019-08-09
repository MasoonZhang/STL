#pragma once

#include "allocator.h"
#include "uninitialized.h"
#include "slist_iterator.h"

namespace STL {
	
	template <class T, class Alloc = simpleAlloc<T> >
	class slist {

	public:
		using value_type = T;
		using pointer = value_type *;
		using const_pointer = const value_type*;
		using reference = value_type &;
		using const_reference = const value_type &;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using iterator = __slist_iterator<T, T&, T*>;
		using const_iterator = __slist_iterator<T, const T&, const T*>;

	private:
		using list_node = __slist_node<T>;
		using list_node_base = __slist_node_base;
		using iterator_base = __slist_iterator_base;
		using list_node_allocator = simpleAlloc<list_node>;

		static list_node* create_node(const value_type& x) {
			list_node* node = list_node_allocator::allocate();
			__STL_TRY{
				construct(&node->data, x);
				node->next = nullptr;
			}
			__STL_UNWIND(list_node_allocator::deallocate(node));
			return node;
		}

		static void destroy_node(list_node* node) {
			destroy(&node->data);
			list_node_allocator::deallocate(node);
		}

	private:
		list_node_base head;

	public:
		slist() { head.next = nullptr; }
		~slist() { clear(); }

	public:
		iterator begin() { return iterator((list_node*)head.next); }
		iterator end() { return iterator(nullptr); }
		size_type size() const noexcept { return __slist_size(head.next); }
		bool empty() const noexcept { return head.next == nullptr; }

		void swap(slist& L) {
			list_node_base* tmp = head.next;
			head.next = L.head.next;
			L.head.next = tmp;
		}

	public:
		reference front() { return ((list_node*)head.next)->data; }

		void push_front(const value_type& x) {
			__slist_make_link(&head, create_node(x));
		}

		void pop_front() {
			list_node* node = (list_node*)head.next;
			head.next = node->next;
			destroy_node(node);
		}
	};
}
