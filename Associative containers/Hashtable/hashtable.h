#pragma once

#include <cstddef>
#include "stl_algo.h"
#include "stl_vector.h"

namespace STL {

	template <class Value>
	struct __hashtable_node {
		__hashtable_node* next;
		Value val;
	};

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
	struct __hashtable_iterator {
		using hashtable = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		using iterator = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		using const_iterator = __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		using node = __hashtable_node<Value>;

		using iterator_category = forward_iterator_tag;
		using value_type = Value;
		using difference_type = ptrdiff_t;
		using size_type = size_t;
		using reference = Value &;
		using pointer = Value *;

		node* cur;
		hashtable* ht;

		__hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) { }
		__hashtable_iterator() { };
		reference operator*() const noexcept { return cur->val; }
		pointer operator->() const noexcept { return &(operator*()); }
		iterator& operator++() noexcept;
		iterator operator++(int) noexcept;
		bool operator==(const iterator& it) const noexcept { return cur == it.cur; }
		bool operator!=(const iterator& it) const noexcept { return cur != it.cur; }
	};

	template <class V, class K, class HF, class ExK, class EqK, class A>
	__hashtable_iterator<V, K, HF, ExK, EqK, A>&
		__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++() noexcept {
		const node* old = cur;
		cur = cur->next;
		if (!cur) {
			size_type bucket = ht->bkt_num(old->val);
			while (!cur && ++bucket < ht->bucket.size())
				cur = ht->buckets(bucket);
		}
		return *this;
	}

	template <class V, class K, class HF, class ExK, class EqK, class A>
	__hashtable_iterator<V, K, HF, ExK, EqK, A>
		__hashtable_iterator<V, K, HF, ExK, EqK, A>::operator++(int) noexcept {
		iterator tmp = *this;
		++*this;
		return tmp;
	}

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
	struct __hashtable_const_iterator {
		using hashtable = hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		using iterator = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		using const_iterator = __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
		using node = __hashtable_node<Value>;

		using iterator_category = forward_iterator_tag;
		using value_type = Value;
		using difference_type = ptrdiff_t;
		using size_type = size_t;
		using reference = Value &;
		using pointer = Value *;

		node* cur;
		hashtable* ht;

		__hashtable_const_iterator(const node* n,  const hashtable* tab) : cur(n), ht(tab) { }
		__hashtable_const_iterator() { };
		__hashtable_const_iterator(const iterator& it) : cur(it.cur), ht(it.ht) { }
		reference operator*() const noexcept { return cur->val; }
		pointer operator->() const noexcept { return &(operator*()); }
		const_iterator& operator++() noexcept;
		const_iterator operator++(int) noexcept;
		bool operator==(const const_iterator& it) const noexcept { return cur == it.cur; }
		bool operator!=(const const_iterator& it) const noexcept { return cur != it.cur; }
	};

	template <class V, class K, class HF, class ExK, class EqK, class A>
	__hashtable_const_iterator<V, K, HF, ExK, EqK, A>&
		__hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++() noexcept {
		const node* old = cur;
		cur = cur->next;
		if (!cur) {
			size_type bucket = ht->bkt_num(old->val);
			while (!cur && ++bucket < ht->bucket.size())
				cur = ht->buckets(bucket);
		}
		return *this;
	}

	template <class V, class K, class HF, class ExK, class EqK, class A>
	__hashtable_const_iterator<V, K, HF, ExK, EqK, A>
		__hashtable_const_iterator<V, K, HF, ExK, EqK, A>::operator++(int) noexcept {
		iterator tmp = *this;
		++* this;
		return tmp;
	}

	enum { __stl_num_primes = 28 };
	static const unsigned long __stl_prime_list[__stl_num_primes] = {
		53ul,         97ul,         193ul,       389ul,       769ul,
		1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
		49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
		1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
		50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
		1610612741ul, 3221225473ul, 4294967291ul
	};

	inline unsigned long __stl_next_prime(unsigned long n) noexcept {
		const unsigned long* first = __stl_prime_list;
		const unsigned long* last = __stl_prime_list + __stl_num_primes;
		const unsigned long* pos = lower_bound(first, last, n);
		return pos == last ? *(last - 1) : *pos;
	}

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc = simpleAlloc<Value> >
	class hashtable {
	public:
		using hasher = HashFcn;
		using key_equal = EqualKey;
		using size_type = size_t;

	private:
		hasher hash;
		key_equal equals;
		ExtractKey get_key;

		using node = __hashtable_node<Value>;
		using node_allocator = simpleAlloc<node, Alloc>;

		vector<node*, Alloc> buckets;
		size_type num_elements;

	private:
		node* new_node(const value_type& obj) {
			node* n = node_allocator::allocate();
			n->next = nullptr;
			try {
				construct(&n->val, obj);
				return n;
			}
			catch (std::exception&) {
				node_allocator::deallocate(n);
			}
		}

		void delete_node(node* n) {
			destroy(&n->val);
			node_allocator::deallocate(n);
		}

	private:
		void initialize_buckets(size_type n) {
			const size_type n_buckets = next_size();
			buckets.reserve(n_buckets);
			buckets.insert(buckets.end(), n_buckets, static_cast<node*>(nullptr));
			num_elements = 0;
		}
		size_type next_size(size_type n) const { return __stl_next_prime(n); }

		size_type bkt_num(const value_type& obj, size_t n) const noexcept {
			return bkt_num_key(get_key(obj), n);
		}
		size_type bkt_num(const value_type& obj) const noexcept {
			return bkt_num_key(get_key(obj));
		}
		size_type bkt_num_key(const key_type& key) const noexcept {
			return bkt_num_key(key, buckets.size());
		}
		size_type bkt_num_key(const key_type& key, size_t n) const noexcept {
			return hash(key) % n;
		}

	private:
		pair<iterator, bool> insert_unique_noreseize(const value_type& obj) {
			const size_type n = bkt_num(obj);
			node* first = buckets[n];

			for (node* cur = first; cur; cur = cur->next) {
				if (equals(get_key(cur->val), get_key(obj)))
					return pair<iterator, bool>(iterator(cur, this), false);
			}

			node* tmp = new_node(obj);
			tmp->next = first;
			buckets[n] = tmp;
			++num_elements;
			return pair<iterator, bool>(iterator(tmp, this), true);
		}
		iterator insert_equal_noresize(const value_type& obj) {
			const size_type n = bkt_num(obj);
			node* first = buckets[n];
			for (node* cur = first; cur; cur = cur->next) {
				if (equals(get_key(cur->val), get_key(obj))) {
					node* tmp = new_node(obj);
					tmp->next = cur->next;
					cur->next = tmp;
					++num_elements;
					return iterator(tmp, this);
				}
			}
			node* tmp = new_node(obj);
			tmp->next = first;
			buckets[n] = tmp;
			++num_elements;
			return iterator(tmp, this);
		}

	public:
		hashtable(size_type n, const hasher& hf, const key_equal& eql)
			: hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
			initialize_buckets(n);
		}	

	public:
		void resize(size_type);

	public:
		iterator find(const key_type& key) {
			size_type n = bkt_num_key(key);
			node* first;
			for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next) { }
			return iterator(first, this);
		}
		size_type count(const key_type& key) const {
			const size_type n = bkt_num_key(key);
			size_type result = 0;
			for (const node* cur = buckets[n]; cur; cur = cur->next)
				if (equals(get_key(cur->val), key))
					++result;
			return result;
		}

	public:
		pair<iterator, bool> insert_unique(const value_type& obj) {
			resize(num_elements + 1);
			return insert_unique_noresize(obj);
		}
		iterator insert_equal(const value_type&) {
			resize(num_elements + 1);
			return insert_equal_noresize(obj);
		}
		void copy_from(const hashtable&);

	public:
		void clear();

	public:
		size_type bucket_count() const { return buckets.size(); }
		size_type max_bucket_count() const {
			return __stl_prime_list[__stl_num_primes - 1];
		}
	};

	template <class V, class K, class HF, class Ex, class Eq, class A>
	void hashtable<V, K, HF, Ex, Eq, A>::resize(size_type num_elements_hint) {
		const size_type old_n = buckets.size();
		if (num_elements_hint > old_n) {
			const size_type n = next_size(num_elements_hint);
			if (n > old_n) {
				vector<node*> tmp(n, static_cast<node*>(nullptr));
				try {
					for (size_type bucket = 0; bucket < old_n; ++bucket) {
						node* first = buckets[bucket];
						while (first) {
							size_type new_bucket = bkt_num(first->val, n);
							buckets[bucket] = first->next;
							first->next = tmp[new_bucket];
							tmp[new_bucket] = first;
							first = buckets[bucket];
						}
					}
					buckets.swap(tmp);
				}
				catch (std::exception&) {
					clear();
				}
			}
		}
	}

	template <class V, class K, class HF, class Ex, class Eq, class A>
	inline void hashtable<V, K, HF, Ex, Eq, A>::copy_from(const hashtable& ht) {
		buckets.clear();
		buckets.reserve(ht.buckets.size());
		buckets.insert(buckets.end(), ht.buckets.size(), static_cast<node*>(nullptr));
		try {
			for (size_type i = 0; i < ht.buckets.size(); ++i) {
				if (const node * cur = ht.buckets[i]) {
					node* copy = new_node(cur->val);
					buckets[i] = copy;

					for (node* next = cur->next; next; cur = next, next = next->next) {
						copy->next = new_node(next->val);
						copy = copy->next;
					}
				}
			}
			num_elements = ht.num_elements;
		}
		catch (std::exception&) {
			clear();
		}
	}

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>::clear() {
		for (size_type i = 0; i != buckets.size(); ++i) {
			node* cur = buckets[i];
			while (cur != null) {
				node* next = cur->next;
				delete_node(cur);
				cur = next;
			}
			buckets[i] = nullptr;
		}
		num_elements = 0;
	}
}	
