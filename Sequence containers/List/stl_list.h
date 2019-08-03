#pragma once

#include "allocator.h"
#include "uninitialized.h"
#include "stl_list_iterator.h"

namespace STL {

	template <class T, class Alloc = simpleAlloc<T> >
	class list {

	private:
		using list_node = __list_node<T>;
		using list_node_allocator = simpleAlloc<list_node>;

	public:
		using link_type = list_node *;
		using size_type = size_t;
		using value_type = T;
		using reference = value_type &;

		using iterator = __list_iterator<T>;

	private:
		link_type node;
		link_type get_node() { return list_node_allocator::allocate(); }
		void put_node(link_type p) { list_node_allocator::deallocate(); }

		link_type create_node(const& T x) {
			link_type p = get_node();
			construct(&p->data, x);
			return p;
		}
		void destroy_node(link_type p) {
			destroy(&p->data);
			put_node(p);
		}

	private:
		void empty_initialize() {
			node = get_node();
			node->next = node;
			node->prev = node;
		}
		void transfer(iterator position, iterator first, iterator last) {
			if (position != last) {
				last.node->prev->next = position.node;
				first.node->prev->next = last.node;
				position.node->prev->next = first.node;
				list_node* tmp = position.node->prev;
				position.node->prev = last.node->prev;
				last.node->prev = first.node->prev;
				first.node->prev = tmp;
			}
		}

	public:
		list() { empty_initialize(); }

	public:
		void swap(list& x) noexcept { STL::swap(node, x.node); }

	public:
		iterator begin() noexcept { return static_cast<link_type>((*node).next); }
		iterator end() noexcept { return node; }
		bool empty() const noexcept { return node->next == node; }
		size_type size() const noexcept {
			size_type result = 0;
			distance(begin(), end(), result);
			return result;
		}
		reference front() noexcept { return *begin(); }
		reference back() noexcept { return *end(); }

		iterator insert(iterator position, const T& x) {
			link_type tmp = create_node(x);
			tmp->next = position.node;
			tmp->prev = position.node->prev;
			static_cast<link_type>(position.node->prev)->next = tmp;
			position.node->prev = tmp;
			return tmp;
		}

		void push_front(const T& x) { insert(begin(), x); }
		void push_back(const T& x) { insert(end(), x); }

		iterator erase(iterator position) {
			link_type next_node = static_cast<link_type>(position.node->next);
			link_type prev_node = static_cast<link_type>(position.node->prev);
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destroy_node(position.node);
			return static_cast<iterator>(next_node);
		}
		void clear();

		void pop_front() { erase(begin()); }
		void pop_back() {
			iterator tmp = end();
			erase(--tmp);
		}

		void unique();
		void splice(iterator position, list& x) {
			if (!x.empty()) {
				transfer(position, x.begin(), x.end());
			}
		}
		void splice(iterator position, list&, iterator i) {
			iterator j = i;
			++j;
			if (position == i || position == j) return;
			transfer(position, i, j);
		}
		void splice(iterator position, list&, iterator first, iterator last) {
			if (first != last)
				transfer(position, first, last);
		}
		void merge(list<T, Alloc>& x);
		void reverse();
		void sort();
		void remove(const T& value);
	};

	template <class T, class Alloc>
	void list<T, Alloc>::clear() {
		link_type cur = static_cast<link_type>(node->next);
		while (cur != node) {
			link_type tmp = cur;
			cur = static_cast<link_type>(cur->next);
			destroy_node(tmp);
		}
		node->next = node;
		node->prev = node;
	}

	template <class T, class Alloc>
	void list<T, Alloc>::unique() {
		iterator first = begin();
		iterator last = end();
		if (first == last) return;
		iterator next = first;
		while (++next != last) {
			if (*first == *next)
				erase(next);
			else
				first = next;
			next = first;
		}
	}

	template <class T, class Alloc>
	void list<T, Alloc>::merge(list<T, Alloc>& x) {
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = begin();
		iterator last2 = end();

		while(first1 != last1 && first2 != last2)
			if (*first2 < *first1) {
				iterator next = first2;
				transfer(first1, first2, ++next);
				first2 = next;
			}
			else
				++first1;
		if (first2 != last2) transfer(last1, first2, last2);
	}

	template <class T, class Alloc>
	void list<T, Alloc>::reverse() {
		if (node->next == node || static_cast<link_type>(node->next)->next == node)
			return;
		iterator first = begin();
		++first;
		while (first != end()) {
			iterator olf = first;
			++first;
			transfer(begin(), old, first);
		}
	}

	template <class T, class Alloc>
	void list<T, Alloc>::sort() {
		if (node->next == node || static_cast<link_type>(node->next)->next == node)
			return;

		list<T, Alloc> carry;
		list<T, Alloc> counter[64];
		int fill = 0;
		while (!empty()) {
			carry.splice(carry.begin(), *this, begin());
			int i = 0;
			while (i < fill && !counter[i].empty()) {
				counter[i].merge(carry);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill) ++fill;
		}

		for (int i = 1; i < fill; ++i)
			counter[i].merge(counter[i - 1]);
		swap(counter[fill - 1]);
	}

	template <class T, class Alloc>
	void list<T, Alloc>::remove(const T& value) {
		iterator first = begin();
		iterator last = end();
		while (first != last) {
			iterator next = first;
			++next;
			if (*first == value) erase(first);
			first = next;
		}
	}
}
