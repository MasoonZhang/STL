#pragma once

#include "allocator.h"
#include "uninitialized.h"
#include "deque_iterator.h"

namespace STL {

	template <class T, class Alloc = simpleAlloc<T>, size_t Bufsiz = 0>
	class deque {

	public:
		using value_type = T;
		using pointer = value_type *;
		using size_type = size_t;
	
	public:
		using iterator = __deque_iterator<T, T&, Bufsiz>;

	private:
		using map_pointer = pointer *;
		using node_allocator = simpleAlloc<value_type, Alloc>;
		using map_allocator = simpleAlloc<pointer, Alloc>;

	private:
		iterator start;
		iterator finish;

		map_pointer map;

		size_type map_size;

	private:
		value_type* allocate_node() {
			return node_allocator::allocate(__deque_buf_size(sizeof(value_type)));
		}
		void deallocate_node(value_type* p) {
			node_allocator::deallocate(p, __deque_buf_size(sizeof(value_type)));
		}
		void destroy_nodes(map_pointer, map_pointer);

	private:
		size_type initial_map_size() const noexcept { return 8U; }
		size_type buffer_size() const noexcept { return iterator::buffer_size(); }
		void fill_initialize(size_type n, const value_type value);
		void create_map_and_nodes(size_type num_elements);

	private:
		void reallocate_map(size_type nodes_to_add, bool add_at_front);
		void reserve_map_at_back(size_type nodes_to_add = 1);
		void reserve_map_at_front(size_type nodes_to_add = 1);

	public:
		deque(int n, const value_type& value) : start(), finish(), map(0), map_size(0) {
			fill_initialize(n, value);
		}

	public:
		iterator begin() noexcept { return start; }
		iterator end() noexcept { return finish; }
		reference operator[](size_type n) {
			return start[difference_type(n)];
		}

		reference front() noexcept { return *start; }
		reference back() noexcept {
			iterator tmp = finish;
			--tmp;
			return *tmp;
		}

		size_type size() const noexcept { return finish - start; }
		size_type max_size() const noexcept { return size_type(-1); }
		bool empty() const noexcept { return finish == start; }

	private:
		void push_back_aux(const value_type&);
		void push_front_aux(const value_type&);
		void pop_back_aux();
		void pop_front_aux();

	public:
		inline void push_back(const value_type& t) {
			if (finish.cur != finish.last - 1) {
				construct(finish.cur, t);
				++finish.cur;
			}
			else
				push_back_aux(t);
		}
		inline void push_front(const value_type& t) {
			if (start.cur != start.first) {
				construct(start.cur - 1, t);
				--start.cur;
			}
			else
				push_front_aux(t);
		}
		inline void pop_back() {
			if (finish.cur != finish.first) {
				--finish.cur;
				destroy(finish.cur);
			}
			else
				pop_back_aux();
		}
		inline void pop_front() {
			if (start.cur != start.last - 1) {
				destroy(start.cur);
				++start.cur;
			}
			else
				pop_front_aux();
		}

	private:
		iterator insert_aux(iterator pos, const value_type& x);

	public:
		iterator insert(iterator position, const value_type& x);

	public:
		iterator erase(iterator pos);
		iterator erase(iterator first, iterator last);
		void clear();
	};

	template <class T, class Alloc, size_t Bufsiz>
	inline void deque<T, Alloc, Bufsiz>::reallocate_map(size_type nodes_to_add,
		bool add_at_front) {
		size_type old_num_nodes = finish.node - start.node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		if (map_size > 2 * new_num_nodes) {
			new_nstart = map + (map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			if (new_nstart < start.node)
				STL::copy(start.node, finish.node + 1, new_nstart);
			else
				STL::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
		}
		else {
			size_type new_map_size = map_size + STL::max(map_size, nodes_to_add) + 2;
			map_pointer new_map = map_allocator::allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			STL::copy(start.node, finish.node + 1, new_nstart);
			map_allocator::deallocate(map, map_size);
			map = new_nstart;
			map_size = new_map_size;
		}

		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes);
	}

	template <class T, class Alloc, size_t Bufsiz>
	inline void deque<T, Alloc, Bufsiz>::reserve_map_at_back(size_type nodes_to_add) {
		if (nodes_to_add + 1 > map_size - (finish.node - map))
			reallocate_map(nodes_to_add, false);
	}

	template <class T, class Alloc, size_t Bufsiz>
	inline void deque<T, Alloc, Bufsiz>::reserve_map_at_front(size_type nodes_to_add) {
		if (nodes_to_add > static_cast<size_type>(start.node - map))
			reallocate_map(nodes_to_add, true);
	}

	template <class T, class Alloc, size_t Bufsiz>
	inline void deque<T, Alloc, Bufsiz>::push_back_aux(const value_type& t) {
		value_type t_copy = t;
		reserve_map_at_back();
		*(finish.node + 1) = allocate_node();
		__STL_TRY{
			construct(finish.cur, t_copy);
			finish.set_node(finish.node + 1);
		}
		__STL_UNWIND(deallocate_node(*(finish.node + 1)));
	}

	template <class T, class Alloc, size_t Bufsiz>
	inline void deque<T, Alloc, Bufsiz>::push_front_aux(const value_type& t) {
		value_type t_copy = t;
		reserve_map_at_front();
		*(start.node - 1) = allocate_node();
		try {
			start.set_node(start.node - 1);
			start.cur = start.last - 1;
			construct(start.cur, t_copy);
		}
		catch (std::exception&) {
			start.set_node(start.node + 1);
			start.cur = start.first;
			deallocate_node(*(start.node - 1));
			throw;
		}
	}

	template <class T, class Alloc, size_t Bufsiz>
	inline void deque<T, Alloc, Bufsiz>::pop_back_aux() {
		deallocate_node(finish.first);
		finish.set_node(finish.node - 1);
		finish.cur = finish.last - 1;
		destroy(finish.cur);
	}

	template <class T, class Alloc, size_t Bufsiz>
	inline void deque<T, Alloc, Bufsiz>::pop_front_aux() {
		destroy(start.cur);

		deallocate_node(start.first);
		start.set_node(start.node + 1);
		start.cur = start.first;
	}

	template <class T, class Alloc, size_t Bufsiz>
	void deque<T, Alloc, Bufsiz>::destroy_nodes(map_pointer nstart, map_pointer nfinish) {
		for (map_pointer n = nstart; n < nfinish; ++n)
			deallocate_node(*n);
	}

	template <class T, class Alloc, size_t Bufsiz>
	void deque<T, Alloc, Bufsiz>::create_map_and_nodes(size_type num_elementes) {
		size_type num_nodes = num_elementes / buffer_size() + 1;

		map_size = STL::max(initial_map_size(), num_nodes + 2);
		map = map_allocator::allocate(map_size);

		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur;
		__STL_TRY {
			for (cur = nstart; cur <= nfinish; ++cur)
				* cur = allocate_node();
		}
		catch (std::exception&) {
			destroy_nodes(nstart, cur);
			throw;
		}

		start.set_node(nstart);
		finish.set_node(nfinish);
		start.cur = start.first;
		finish.cur = finish.first + num_elementes % buffer_size();
	}

	template <class T, class Alloc, size_t Bufsiz>
	void deque<T, Alloc, Bufsiz>::fill_initialize(size_type n, const value_type value) {
		create_map_and_nodes(n);
		map_pointer cur;
		__STL_TRY {
			for (cur = start.node; cur < finish.node; ++cur)
				STL::uninitialized_fill(*cur, *cur + buffer_size(), value);
			STL::uninitialized_fill(finish.first, finish.cur, value);
		}
		catch (std::exception&) {
			destroy(start, iterator(*cur, cur));
			throw;
		}
	}

	template <class T, class Alloc, size_t Bufsiz>
	typename deque<T, Alloc, Bufsiz>::iterator deque<T, Alloc, Bufsiz>::insert_aux(iterator pos, const value_type& x) {
		difference_type index = pos - start;
		value_type x_copy = x;
		if (index < size() / 2) {
			push_front(front());
			iterator front1 = start;
			++front1;
			iterator front2 = front1;
			++front2;
			pos = start + index;
			iterator pos1 = pos;
			++pos1;
			STL::copy(front2, pos1, front1);
		}
		else {
			push_back(back());
			iterator back1 = finish;
			--back1;
			iterator back2 = back1;
			--back2;
			pos = start + index;
			STL::copy(pos, back2, back1);
		}
		*pos = x_copy;
		return pos;
	}

	template <class T, class Alloc, size_t Bufsiz>
	typename deque<T, Alloc, Bufsiz>::iterator deque<T, Alloc, Bufsiz>::insert(iterator position, const value_type& x) {
		if (position.cur == start.cur) {
			push_front(x);
			return start;
		}
		else if (position.cur == finish.cur) {
			push_back(x);
			iterator tmp = finish;
			--tmp;
			return tmp;
		}
		else
			return insert_aux(position, x);
	}

	template <class T, class Alloc, size_t Bufsiz>
	typename deque<T, Alloc, Bufsiz>::iterator deque<T, Alloc, Bufsiz>::erase(iterator pos) {
		iterator next = pos;
		++next;
		difference_type index = pos - start;
		if (index < (size() >> 1)) {
			STL::copy_backward(start, pos, next);
			pop_front();
		}
		else {
			STL::copy(next, finish, pos);
			pop_back();
		}
		return start + index;
	}

	template <class T, class Alloc, size_t Bufsiz>
	typename deque<T, Alloc, Bufsiz>::iterator deque<T, Alloc, Bufsiz>::erase(iterator first, iterator last) {
		if (first == start && last == finish) {
			clear();
			return finish;
		}
		else {
			difference_type n = last - first;
			difference_type elems_before = first = start;
			if (elems_before < (size() - n) / 2) {
				STL::copy_backward(start, first, last);
				iterator new_start = start + n;
				destroy(start, new_start);
				for (map_pointer cur = start.node; cur < new_start.node; ++cur)
					node_allocator::deallocate(*cur, buffer_size());
				start = new_start;
			}
			else {
				STL::copy_backward(last, finish, first);
				iterator new_finish = finish - n;
				destroy(new_finish, finish);
				for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
					node_allocator::deallocate(*cur, buffer_size());
				finish = new_finish;
			}
			return start + elems_before;
		}
	}

	template <class T, class Alloc, size_t Bufsiz>
	void deque<T, Alloc, Bufsiz>::clear() {
		for (map_pointer node = start.node + 1; node < finish.node; ++node) {
			destroy(*node, *node + buffer_size());
			node_allocator::deallocate(*node, buffer_size());
		}

		if (start.node != finish.node) {
			destroy(start.cur, start.last);
			destroy(finish.first, finish.cur);
			node_allocator::deallocate(finish.first, buffer_size());
		}
		else
			destroy(start.cur, start.last);
		finish = start;
	}
}
