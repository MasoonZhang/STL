#pragma once

#include "hash_func.h"
#include "hashtable.h"


namespace STL {

	template <class Value, class HashFcn = hash<Value>, class EqualKey = equal_to<Value>, class Alloc = simpleAlloc<Value> >
	class hash_set {
	private:
		using ht = hashtable<Value, Value, HashFcn, identity<Value>, EqualKey, Alloc>;
		ht rep;

	public:
		using key_type = typename ht::key_type;
		using value_type = typename ht::value_type;
		using hasher = typename ht::hasher;
		using key_equal = typename ht::key_equal;

		using size_type = typename ht::size_type;
		using differece_type = typename ht::differece_type;
		using pointer = typename ht::const_pointer;
		using const_pointer = typename ht::const_pointer;
		using iterator = typename ht::const_iterator;
		using const_iterator = typename ht::const_iterator;

		hasher hash_funct() const noexcept { return rep.hash_funct(); }
		key_equal key_eq() const noexcept { return rep.key_eq(); }

	public:
		hash_set() : rep(100, hasher(), key_equal()) { }
		explicit hash_set(size_type n) : rep(n, hasher(), key_equal()) { }
		hash_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) { }
		hash_set(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) { }

		template <class InputIterator>
		hash_set(InputIterator f, InputIterator l)
			: rep(100, hasher(), key_equal()) {
			rep.insert_unique(f, l);
		}
		template <class InputIterator>
		hash_set(InputIterator first, InputIterator last, size_type n)
			: rep(n, hasher(), key_equal()) {
			rep.insert_unique(first, last);
		}
		template <class InputIterator>
		hash_set(InputIterator first, InputIterator last, size_type n, const hasher& hf)
			: rep(n, hf, key_equal()) {
			rep.insert_unique(first, last);
		}
		template <class InputIterator>
		hash_set(InputIterator first, InputIterator last, size_type n, const hasher& hf, const key_equal& eql)
			: rep(n, hf, eql()) {
			rep.insert_unique(first, last);
		}

	public:
		size_type size() const noexcept { return rep.size(); }
		size_type max_size() const noexcept { return rep.max_size(); }
		bool empty() const noexcept { return rep.empty(); }
		void swap(hash_set& ht) noexcept { rep.swap(ht.rep); }
		friend bool operator==(const hash_set&, const hash_set&);
		
		iterator begin() const noexcept { return rep.begin(); }
		iterator end() const noexcept { return rep.end(); }

	public:
		pair<iterator, bool> insert(const value_type& obj) {
			pair<typename ht::iterator, bool> p = rep.insert_unique(obj);
			return pair<iterator, bool>(p.first, p.second);
		}
		template <class InputIterator>
		void insert(InputIterator first, InputIterator last) {
			rep.insert_unique(first, last);
		}
		pair<iterator, bool> insert_noresize(const value_type& obj) {
			pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(obj);
			return pair<iterator, bool>(p.first, p.second);
		}

		iterator find(const key_type& key) const { return rep.find(key); }

		size_type count(const key_type& key) const { return rep.count(key); }

		pair<iterator, iterator> equal_range(const key_type& key) const {
			return rep.equal_range(key);
		}

		size_type erase(const key_type& key) { return rep.erase(key); }
		void erase(iterator it) { rep.erase(it); }
		void erase(iterator first, iterator last) { rep.erase(first, last); }

		void clear() { rep.clear(); }

	public:
		void resize(size_type n) { rep.resize(n); }
		size_type bucket_count() const noexcept { return rep.bucket_count(); }
		size_type max_bucket_count() const noexcept { return rep.max_bucket_count(); }
		/*size_type elems_in_bucket(size_type n) const noexcept {
			return rep.elems_in_bucket(n);
		}*/
	};

	template <class Value, class HashFcn, class EqualKey, class Alloc>
	inline bool operator==(const hash_set<Value, HashFcn, EqualKey, Alloc>& lhs,
		const hash_set<Value, HashFcn, EqualKey, Alloc>& rhs) {
		return lhs.rep == rhs.rep;
	}
}
