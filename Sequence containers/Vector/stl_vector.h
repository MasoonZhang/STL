#pragma once
 
#include <cstddef>
#include "allocator.h"
#include "uninitialized.h"

namespace STL {

	template <class T, class Alloc = simpleAlloc<T> >
	class vector {

	public:
		using value_type = T;
		using pointer = value_type *;
		using iterator = value_type *;
		using reference = value_type &;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

	private:
		iterator start;
		iterator finish;
		iterator end_of_storage;

	private:
		using data_allocator = Alloc;

		void insert_aux(iterator position, const T& x);
		void deallocate() {
			if (start) data_allocator::deallocate(start, end_of_storage - start);
		}
		void fill_initialize(size_type n, const T& value) {
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}

		iterator allocate_and_fill(size_type n, const T & value) {
			iterator result = data_allocator::allocate(n);
			STL::unintialized_fill_n(result, n, value)
			return result;
		}

		void fill_initialize(size_type n, const T& value) {
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}

	public:
		iterator begin() noexcept { return start; }
		iterator end() noexcept { return finish;  }
		size_type size() { return return(finish - start); }
		size_type capacity() const noexcept { return static_cast<size_type>(end_of_storage - start); }
		bool empty() const noexcept { return start == finish; }
		reference operator[](size_type n) { return *(start + n); }

		vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) { }
		vector(size_type n, const T& value) { fill_initialize(n, value); }
		vector(int n, const T& value) { fill_initialize(n, value); }
		vector(long n, const T& value) { fill_initialize(n, value); }
		explicit vector(size_type n) { fill_initialize(n, T()); }

		~vector() {
			destroy(start, finish);
			deallocate();
		}
		reference front() noexcept { return *start; }
		reference back() noexcept { return *finish; }
		void push_back(const T& x) {
			if (finish != end_of_storage) {
				construct(finish, x);
				++finish;
			}
			else
				insert_aux(finish, x);
		}

		void pop_back() {
			--finish;
			destroy(finish);
		}

		iterator erase(iterator position) {
			if (position + 1 != finish)
				copy(position + 1, finish, position);
			--finish;
			destroy(finish);
			return position;
		}
		void resize(size_type new_size, const T& x) {
			if (new_size < size())
				erase(start + new_size, finish);
			else
				insert(finish, new_size - finish, x);
		}
		void resize(size_type new_size) { return resize(new_size, T()); }
		void clear() { erase(start, finish); }
		void insert(iterator position, size_type n, const T& x) {
			if (n != 0) {
				if (size_type(end_of_storage - finish >= n)) {
					T x_copy = x;
					const size_type elems_after = finish - position;
					iterator old_finish = finish;
					if (elems_after > n) {
						uninitialized_copy(finish - n, finish, finish);
						finish += n;
						copy_backward(position, old_finish - n, old_finish);
						fill(position, position + n, x_copy);
					}
					else {
						uninitialized_fill_n(finish, n - elems_after, x_copy);
						finish += n - elems_after;
						uninitialized_copy(position, old_finish, finish);
						finish += elems_after;
						fill(position, old_finish, x_copy);
					}
				}
				else {
					const size_type old_size = size();
					const size_type len = old_size + max(old_size, n);
					iterator new_start = data_allocator::allocate(len);
					iterator new_finish = new_start;
					__STL_TRY{
						new_finish = uninitialized_copy(start, position, new_start);
						new_finish = uninitialized_fill_n(new_finish, n);
						new_finish = uninitialized_copy(position, finish, new_finish);
					}
				# ifdef __STL_USE_EXCEPTIONS
					catch (...) {
						destroy(new_start, new_finish);
						data_allocator::deallocate(new_start, len);
						throw;
					}
				# endif /* __STL_USE_EXCEPTIONS */
					destroy(start, finish);
					deallocate();
					start = new_start;
					finish = new_finish;
					end_of_storage = new_start + len;
				}
			}
		}
	};
	
	template <class T, class Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
		if (finish != end_of_storage) {
			construct(finish, *(finish - 1))
			++finish;
			T x_copy = x;
			STL::copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size != 0 ? old_size * 2 : 1;

			iterator new_start = data_allocator::allocate(len);
			iterator new_finish = new_start;
			try {
				new_finish = STL::uninitialized_copy(start, position, new_start);
				const construct(new_finish, x);
				++new_finish;
				new_finish = STL::uninitialized_copy(position, finish, new_finish)
			}
			catch (std::exception&) {
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			destroy(start, finish);
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}
