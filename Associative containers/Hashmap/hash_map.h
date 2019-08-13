#pragma once

#include "hash_func.h"
#include "hashtable.h"

namespace STL {

	template <class Key, class T, class HashFcn = hash<Key>, class EqualKey = equal_to<Key>, class Alloc = simpleAlloc<Key> >
	class hash_map {
	private:
		using ht = hashtable<pair<const Key, T>, Key, HashFcn, select1st<pair<const Key, T> >, EqualKey, Alloc>;
		ht rep;

	public:
		using key_type = typename ht::key_type;
		using data_type = T;
		using mapped_type = T;
		using value_type = typename ht::value_type;
		using hasher = typename ht::hasher;
		using key_equal = typename ht::key_equal;

		using size_type = typename ht::size_type;
		using differece_type = typename ht::differece_type;
		using pointer = typename ht::pointer;
		using const_pointer = typename ht::const_pointer;
		using reference = typename ht::reference;
		using const_reference = typename ht::const_reference;

		using iterator = typename ht::iterator;
		using const_iterator = typename ht::const_iterator;

		hasher hash_funct() const noexcept { return rep.hash_funct(); }
		key_equal key_eq() const noexcept { return rep.key_eq(); }

	public:
		hash_map() : rep(100, hasher(), key_equal()) { }
		explicit hash_map(size_type n) : rep(n, hasher(), key_equal()) { }
		hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) { }
		hash_map(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) { }

		template <class InputIterator>
		hash_map(InputIterator f, InputIterator l)
			: rep(100, hasher(), key_equal()) {
			rep.insert_unique(f, l);
		}
		template <class InputIterator>
		hash_map(InputIterator f, InputIterator l, size_type n)
			: rep(n, hasher(), key_equal()) {
			rep.insert_unique(f, l);
		}
		template <class InputIterator>
		hash_map(InputIterator f, InputIterator l, size_type n, const hasher& hf)
			: rep(n, hf, key_equal()) {
			rep.insert_unique(f, l);
		}
		template <class InputIterator>
		hash_map(InputIterator f, InputIterator l, size_type n, const hasher& hf, const key_equal& eql)
			: rep(n, hf, eql()) {
			rep.insert_unique(f, l);
		}

	public:
		size_type size() const noexcept { return rep.size(); }
		size_type max_size() const noexcept { return rep.max_size(); }
		bool empty() const noexcept { return rep.empty(); }
		void swap(hash_map& ht) noexcept { rep.swap(ht.rep); }
		friend bool operator==(const hash_map&, const hash_map&);

		iterator begin() const noexcept { return rep.begin(); }
		iterator end() const noexcept { return rep.end(); }
		const_iterator begin() const noexcept { return rep.begin(); }
		const_iterator end() const noexcept { return rep.end(); }

	public:
		pair<iterator, bool> insert(const value_type& obj) {
			return rep.insert_unique(obj);
		}
		template <class InputIterator>
		void insert(InputIterator f, InputIterator l) {
			rep.insert_unique(f, l);
		}
		pair<iterator, bool> insert_noresize(const value_type& obj) {
			return rep.insert_unique_noresize(obj);
		}

		iterator find(const key_type& key) const { return rep.find(key); }
		const_iterator find(const key_type& key) const { return rep.find(key); }

		/*T& operator[](const key_type& key) {
			return rep.find_or_insert(value_type(key, T())).second;
		}*/

		size_type count(const key_type& key) const { return rep.count(key); }

		pair<iterator, iterator> equal_range(const key_type& key) const {
			return rep.equal_range(key);
		}
		pair<const_iterator, iterator> equal_range(const key_type& key) const {
			return rep.equal_range(key);
		}

		size_type erase(const key_type& key) { return rep.erase(key); }
		void erase(iterator it) { rep.erase(it); }
		void erase(iterator f, iterator l) { rep.erase(f, l); }
		void clear() { rep.clear(); }

	public:
		void resize(size_type n) { rep.resize(n); }
		size_type bucket_count() const noexcept { return rep.bucket_count(); }
		size_type max_bucket_count() const noexcept { return rep.max_bucket_count(); }
		/*size_type elems_in_bucket(size_type n) const noexcept {
			return rep.elems_in_bucket(n);
		}*/
	};

	template <class Key, class T, class HashFcn, class EqualKey, class Alloc>
	inline bool operator==(const hash_map<Key, T, HashFcn, EqualKey, Alloc>& lhs,
		const hash_map<Key, T, HashFcn, EqualKey, Alloc>& rhs) {
		return lhs.rep == rhs.rep;
	}
}
