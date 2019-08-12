#pragma once

#include "rb_tree.h"
#include "stl_function.h"

namespace STL {

	template <class Key, class T, class Compare = less<key>, class Alloc = alloc>
	class multimap {
		using key_type = Key;
		using data_type = T;
		using mapped_type = T;
		using value_type = pair<const Key, T>;
		using key_compare = Compare;

		class value_compare : public binary_function<value_type, value_type, bool> {
			friend class map<Key, T, Compare, Alloc>;
		protected:
			Compare comp;
			value_compare(Compare c) : comp(c) { }
		public:
			bool operator()(const value_type& x, const value_type& y) const {
				return comp(x.first, y.first);
			}
		};

	private:
		using rep_type = rb_tree<key_type, value_type, select1st<value_type>, key_compare, Alloc>;
		rep_type t;

	public:
		using pointer = typename rep_type::pointer;
		using const_pointer = typename rep_type::const_pointer;
		using reference = typename rep_type::reference;
		using const_reference = typename rep_type::const_reference;
		using iterator = typename rep_type::iterator;
		using const_iterator = typename rep_type::const_iterator;
		using reverse_iterator = typename rep_type::reverse_iterator;
		using const_reverse_iterator = typename rep_type::const_reverse_iterator;
		using size_type = typename rep_type::size_type;
		using difference_type = typename rep_type::difference_type;

		map() : t(Compare()) { }
		explicit multimap(const Compare& comp) : t(comp) { }

		template <class InputIterator>
		multimap(InputIterator first, InputIterator last) : t(Compare()) {
			t.insert_equal(first, last);
		}

		template <class InputIterator>
		multimap(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
			t.insert_equal(first, last);
		}

		multimap(const map<Key, T, Compare, Alloc>& x) : t(x.t) { }
		multimap(const map<Key, T, Compare, Alloc>& operator=(const map(const map<Key, T, Compare, Alloc>& x) {
			t = x.t;
			return *this;
		}

		key_compare key_comp() const noexcept { return t.key_comp(); }
		value_compare value_comp() const noexcept {
			return value_compare(t.key_comp());
		}
		iterator begin() noexcept { return t.begin(); }
		iterator end() noexcept { return t.end(); }
		const_iterator begin() const noexcept { return t.cbegin(); }
		const_iterator end() const noexcept { return t.cend(); }
		const_reverse_iterator crbegin() const noexcept { return t.rbegin(); }
		const_reverse_iterator crend() const noexcept { return t.rend(); }
		bool empty() const noexcept { return t.empty(); }
		size_type size() const noexcept { return t.size(); }
		size_type max_size() const noexcept { return t.max_size(); }
		T& operator[](const key_type& k) {
			return (*(insert(value_type(k, T()))).first()).second;
		}
		void swap(map<Key, T, Compare, Alloc>& x) noexcept { t.swap(x.t); }

		pair<iterator, bool> insert(const value_type& x) {
			return t.insert_equal(x);
		}
		iterator insert(iterator position, const value_type& x) {
			return t.insert_equal(position, x);
		}
		template <class InputIterator>
		void insert(InputIterator first, InputIterator last) {
			t.insert_equal(first, last);
		}

		void erase(iterator position) { t.erase(position); }
		size_type erase(const key_type& x) { return t.erase(x); }
		void erase(iterator first, iterator last) { t.erase(first, last); }
		void clear() { t.clear(); }

		iterator find(const key_type& x) noexcept { return t.find(x); }
		const_iterator find(const key_type& x) const noexcept { return t.find(x); }
		size_type count(const key_type& x) const noexcept { return t.count(x); }
		iterator lower_bound(const key_type& x) noexcept { return t.lower_bound(x); }
		const_iterator lower_bound(const key_type& x) const noexcept {
			return t.lower_bound(x);
		}
		iterator upper_bound(const key_type& x) noexcept { return t.upper_bound(x); }
		const_iterator upper_bound(const key_type& x) const noexcept {
			return t.upper_bound(x);
		}

		pair<iterator, iterator> equal_range(const key_type& x) noexcept {
			return t.equal_range(x);
		}
		pair<const_iterator, const_iterator> equal_range(const key_type& x) const noexcept {
			return t.equal_range(x);
		}
		friend bool operator==(const map&, const map&);
		friend bool operator<(const map&, const map&);
	};

	template <class Key, class Tp, class Compare, class Alloc>
	inline bool operator==(const multimap<Key, Tp, Compare, Alloc>& x, const multimap<Key, Tp, Compare, Alloc>& y) {
		return x.t == y.t;
	}

	template <class Key, class Tp, class Compare, class Alloc>
	inline bool operator<(const multimap<Key, Tp, Compare, Alloc>& x, const multimap<Key, Tp, Compare, Alloc>& y) {
		return x.t < y.t;
	}
}
