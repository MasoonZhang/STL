#pragma once

#include "rb_tree.h"

namespace STL {

	template <class Key, class Compare = less<Key>, class Alloc = simpleAlloc<Key> >
	class set {
	public:
		using key_type = Key;
		using value_type = Key;
		using key_compare = Compare;
		using value_compare = Compare;
	private:
		using  rep_type = rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc>;
		rep_type t;
	public:
		using pointer = typename rep_type::const_pointer;
		using const_pointer = typename rep_type::const_pointer;
		using reference = typename rep_type::const_reference;
		using const_reference = typename rep_type::const_reference;
		using iterator = typename rep_type::const_iterator;
		using const_iterator = typename rep_type::const_iterator;
		using reverse_iterator = typename rep_type::const_reverse_iterator;
		using const_reverse_iterator = typename rep_type::const_reverse_iterator;
		using size_type = typename rep_type::size_type;
		using difference_type = typename rep_type::difference_type;

		set() : t(Compare()) { }
		explicit set(const Compare& comp) : t(comp) { }

		template <class InputIterator>
		set(InputIterator first, InputIterator last) : t(Compare()) {
			t.insert_unique(first, last);
		}

		template <class InputIterator>
		set(InputIterator first, InputIterator last, const Compare& comp) : t(comp) {
			t.insert_unique(first, last);
		}

		set(const set<Key, Compare, Alloc>& x) : t(x.t) { }
		set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) {
			t = x.t;
			return *this;
		}

		key_compare key_comp() const noexcept { return t.key_comp(); }
		value_compare value_comp() const noexcept { return t.key_comp(); }
		iterator begin() const noexcept { return t.begin(); }
		iterator end() const noexcept { return t.end(); }
		reverse_iterator rbegin() const noexcept { return t.rbegin(); }
		reverse_iterator rend() const noexcept { return t.rend(); }
		bool empty() const noexcept { return t.empty(); }
		size_type size() const noexcept { return t.size(); }
		size_type max_size() const noexcept { return t.max_size(); }
		void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }

		using pair_iterator_bool = pair<iterator, bool>;
		pair<iterator, bool> insert(const value_type& x) {
			pair<typename rep_type::iterator, bool> p = t.insert_unique(x);
			return pair<iterator, bool>(p.first, p.second);
		}
		iterator insert(iterator position, const value_type& x) {
			using rep_iterator = typename rep_type::iterator;
			return t.insert_unique(reinterpret_cast<rep_iterator&>(position), x);
		}
		template <class InputIterator>
		void insert(InputIterator first, InputIterator last) {
			t.insert_unique(first, last);
		}
		void erase(iterator position) {
			using rep_iterator = typename rep_type::iterator;
			t.erase(reinterpret_cast<rep_iterator>(position));
		}
		size_type erase(const key_type& x) {
			return t.erase(x);
		}
		void erase(iterator first, iterator last) {
			using rep_iterator = typename rep_type::iterator;
			t.erase(reinterpret_cast<rep_iterator>(first), reinterpret_cast<rep_iterator>(last));
		}
		void clear() noexcept { t.clear(); }

		iterator find(const key_type& x) noexcept { return t.find(x); }
		size_type count(const key_type& x) const noexcept { return t.count(x); }
		iterator lower_bound(const key_type& x) const noexcept {
			return t.lower_bound(x);
		}
		iterator upper_bound(const key_type& x)  const noexcept {
			return t.upper_bound(x);
		}
		pair<iterator, iterator> equal_range(const key_type& x) {
			return t.equal_range(x);
		}

		friend bool operator==(const set&, const set&);
		friend bool operator<(const set&, const set&);
	};

	template <class Key, class Compare, class Alloc>
	inline bool operator==(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
		return x.t == y.t;
	}
	
	template <class Key, class Compare, class Alloc>
	inline bool operator<(const set<Key, Compare, Alloc>& x, const set<Key, Compare, Alloc>& y) {
		return x.t < y.t;
	}
}
