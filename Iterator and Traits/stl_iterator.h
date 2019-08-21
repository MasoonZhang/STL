#pragma once

#include <cstddef>

namespace STL {

	struct input_iterator_tag { };
	struct output_iterator_tag { };
	struct forward_iterator_tag : public input_iterator_tag { };
	struct bidirectional_iterator_tag : public forward_iterator_tag { };
	struct random_access_iterator_tag : public bidirectional_iterator_tag { };

	template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
	struct iterator {
		using iterator_category = Category;
		using value_type = T;
		using difference_type = Distance;
		using pointer = Pointer;
		using reference = Reference;
	};

	template <class Iterator>
	struct iterator_traits {
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
	};

	template <class T>
	struct iterator_traits<T*> {
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T *;
		using reference = T &;
	};

	template <class T>
	struct iterator_traits<const T*> {
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = ptrdiff_t;
		using pointer = const T*;
		using reference = const T &;
	};

	template <class Iterator>
	inline typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&) {
		using iterator_category_t = typename iterator_traits<Iterator>::iterator_category;
		return category();
	}

	template <class Iterator>
	inline typename iterator_traits<Iterator>::difference_type* difference_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	template <class Iterator>
	inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type __distance(InputIterator first, InputIterator last, input_iterator_tag) {
		iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last) {
			++first, ++n;
		}
		return n;
	}

	template <class RandomAccessIterator>
	inline typename iterator_traits<RandomAccessIterator>::difference_type __distance(RandomAccessIterator first,
																						RandomAccessIterator last,
																						random_access_iterator_tag) {
		return last - first;
	}

	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type distance(InputIterator first, InputIterator last) {
		using iterator_category = typename iterator_traits<InputIterator>::iterator_category;
		return __distance(first, last, iterator_category);
	}

	template <class InputIterator, class Distance>
	inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
		while (n--) ++i;
	}

	template <class BidirectionalIterator, class Distance>
	inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
		if (n >= 0)
			while (n--) ++i;
		else
			while (n++) --i;
	}

	template <class BidirectionalIterator, class Distance>
	inline void __advance(BidirectionalIterator& i, Distance n, random_access_iterator_tag) {
		i += n;
	}

	template <class InputIterator, class Distance>
	inline void advance(InputIterator& i, Distance n) {
		__advance(i, n, iteraotr_category(i));
	}

	template <class Container>
	class back_insert_iterator {
	protected:
		Container* container;
	public:
		using iterator_category = output_iterator_tag;
		using value_tyep = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		explicit back_insert_iterator(Container& x) : container(&x) { }
		back_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
			container->push_back(value);
			return *this;
		}
		back_insert_iterator<Container>& operator*() { return *this; }
		back_insert_iterator<Container>& operator++() { return *this; }
		back_insert_iterator<Container>& operator++(int) { return *this; }
	};

	template <class Container>
	inline back_insert_iterator<Container> back_inserter(Container& x) {
		return back_insert_iterator<Container>(x);
	}

	template <class Container>
	class front_insert_iterator {
	protected:
		Container* container;
	public:
		using iterator_category = output_iterator_tag;
		using value_tyep = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		explicit front_insert_iterator(Container& x) : container(&x) { }
		front_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
			container->push_back(value);
			return *this;
		}
		front_insert_iterator<Container>& operator*() { return *this; }
		front_insert_iterator<Container>& operator++() { return *this; }
		front_insert_iterator<Container>& operator++(int) { return *this; }
	};

	template <class Container>
	inline front_insert_iterator<Container> front_inserter(Container& x) {
		return front_insert_iterator<Container>(x);
	}

	template <class Container>
	class insert_iterator {
	protected:
		Container* container;
		typename Container::iterator iter;
	public:
		using iterator_category = output_iterator_tag;
		using value_tyep = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		explicit insert_iterator(Container& x, typename Container::iterator i) : container(&x), iter(i) { }
		insert_iterator<Container>& operator=(const typename Container::value_type& value) {
			iter = container->insert(iter, value);
			++iter;
			return *this;
		}
		insert_iterator<Container>& operator*() { return *this; }
		insert_iterator<Container>& operator++() { return *this; }
		insert_iterator<Container>& operator++(int) { return *this; }
	};

	template <class Container, class Iterator>
	inline insert_iterator<Container> inserter(Container& x, Iterator i) {
		using iter = typename Container::iterator;
		return insert_iterator<Container>(x, iter(i));
	}

	template <class Iterator>
	class reverse_iterator {
	protect:
		Iterator current;
	public:
		using iterator_category = typename iterator_traits<Iterator>::iterator_catogory;
		using value_type = typename iterator_traits<Iterator>::value_type;
		using difference_type = typename iterator_traits<Iterator>::difference_type;
		using pointer = typename iterator_traits<Iterator>::pointer;
		using reference = typename iterator_traits<Iterator>::reference;

		using iterator_type = Iterator;
		using self = reverse_iterator<Iterator>;

	public:
		reverse_iterator() { }
		explicit reverse_iterator(iterator_type x) : current(x) { }
		reverse_iterator(const self& x) : current(x.current) { }

		iterator_type base() const { return current; }
		reference operator*() const {
			Iterator tmp = current;
			return *--tmp;
		}
		pointer operator->() const { return &(operator*()); }

		self& operator++() {
			--current;
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			--current;
			return tmp;
		}
		self& operator--() {
			++current;
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			++current;
			return tmp;
		}
		self operator+(difference_type n) const {
			return self(current - n);
		}
		self& operator+=(difference_type n) const {
			current -= n;
			return *this;
		}
		self operator-(difference_type n) const {
			return self(current + n);
		}
		self& operator-=(difference_type n) const {
			current += n;
			return *this;
		}
		reference operator[](difference_type n) const { return *(*this + n); }
	};

	template <class T, class Distance = ptrdiff_t>
	class istream_iterator {
		//friend bool operator==(const istream_iterator<T, Distance> x, const istream_iterator<T, Distance> y);
	protected:
		istream* stream;
		T value;
		bool end_marker;
		void read() {
			end_marker = (*stream) ? true : false;
			if (end_marker) *stream >> value;
			end_marker = (*stream) ? true : false;
		}
	public:
		using iterator_category = input_iterator_tag;
		using value_tyep = T;
		using difference_type = Distance;
		using pointer = const T*;
		using reference = const T &;

		istream_iterator() : stream(&cin), end_marker(false) { }
		istream_iterator(istream& s) : stream(&s) { read(); }

		reference operator*() const { return value; }
		pointer operator->() const { return &(operator*()); }

		istream_iterator<T, Distance> operator++() {
			read();
			return *this;
		}
		istream_iterator<T, Distance> operator++(int) {
			istream_iterator<T, Distance> tmp = *this;
			read();
			return tmp;
		}
	};

	template <class T>
	class ostream_iterator {
	protected:
		ostream* stream;
		const char* string;

	public:
		using iterator_category = output_iterator_tag;
		using value_tyep = void;
		using difference_type = void;
		using pointer = void;
		using reference = void;

		ostream_iterator(ostream& s) : stream(&s), string(0) { }
		ostream_iterator(ostream& s, const char* c) : stream(&s), string(c) { }

		ostream_iterator<T>& operator=(const T& value) {
			*stream << value;
			if (string) *stream << string;
			return *this;
		}
		ostream_iterator<T>& operator*() { return *this; }
		ostream_iterator<T>& operator++() { return *this; }
		ostream_iterator<T>& operator++(int) { return *this; }
	};
}
