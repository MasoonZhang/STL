#pragma once

#include <new>
#include <string>
#include <cstdlib>

namespace STL {

	template <int inst>
	class __malloc_alloc_template {

	private:
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void(*__malloc_alloc_oom_handler)();

	public:
		static void* allocate(size_t n) {
			void* result = malloc(n);
			if (result == nullptr)
				result = oom_malloc(n);
			return result;
		}	

		static void deallocate(void* p, size_t /* n */) {
			free(p);
		}

		static void* reallocate(void* p, size_t /* old_sz */, size_t new_sz) {
			void* result = realloc(p, new_sz);
			if (result == nullptr)
				result = oom_realloc(p, new_sz);
			return result;
		}

		static void(*set_malloc_handler(void(*f)()))() {
			void (*old)() = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return old;
		}
	};

	template <int inst>
	void(*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = nullptr;

	template <int inst>
	void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
		void(*my_malloc_handler)();
		void* result;

		for (;;) {
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw std::bad_alloc();
			(*my_malloc_handler)();
			result = malloc(n);
			if (result)
				return result;
		}
	}

	template <int inst>
	void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
		void(*my_malloc_handler)();
		void* result;

		for (;;) {
			my_malloc_handler = _malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw std::bad_alloc();
			result = realloc(p, n);
			if (result)
				return result;
		}
	}

	enum { __ALIGN = 8, __MAX_BYTES = 128, __NFREELISTS = __MAX_BYTES / __ALIGN };

	template <bool threads, int inst>
	class __default_alloc_template {

	private:
		static size_t ROUND_UP(size_t bytes) {
			return (bytes + static_cast<size_t>(_ALIGN) - 1) & ~(static_cast<size_t>(_ALIGN) - 1);
		}
	private:
		union obj {
			union obj* free_list_link;
			char client_data[1];
		};
	private:
		static obj* volatile free_list[_NFREELISTS];
		static size_t FREE_LIST_INDEX(size_t bytes) {
			return ((bytes + static_cast<size_t>(_ALIGN) - 1) / static_cast<size_t>(_ALIGN) - 1);
		}
		static void* refill(size_t n);
		static char* chunk_alloc(size_t size, int &nobjs);

		static char* start_free;
		static char* end_free;
		static size_t heap_size;

	public:
		static void* allocate(size_t n);
		static void deallocate(void* p, size_t n);
		static void* reallocate(void* p, size_t old_sz, size_t new_sz);
	};

	template <bool threads, int inst>
	char * __default_alloc_template<threads, inst>::start_free = nullptr;

	template <bool threads, int inst>
	char* __default_alloc_template<threads, inst>::end_free = nullptr;

	template <bool threads, int inst>
	size_t __default_alloc_template<threads, inst>::heap_size = 0;

	template <bool threads, int inst>
	__default_alloc_template<threads, inst>::obj* volatile
		__default_alloc_template<threads, inst>::free_list[_NFREELISTS] = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};

	template <bool threads, int inst>
	void * __default_alloc_template<threads, inst>::allocate(size_t n) {
		obj* volatile *my_free_list;
		obj* result;
		if (n > static_cast<size_t>(_MAX_BYTES)) {
			return __malloc_alloc_template<0>::allocate(n);
		}
		my_free_list = free_list + FREE_LIST_INDEX(n);
		result = *my_free_list;
		if (result == nullptr) {
			void* r = refill(ROUND_UP(n));
			return r;
		}
		*my_free_list = result->free_list_link;
		return result;
	}

	template <bool threads, int inst>
	void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
		obj* q = reinterpret_cast<obj *>(p);
		obj* volatile *my_free_list;

		if (n > (size_t)_MAX_BYTES) {
			__malloc_alloc_template<0>::deallocate(p, n);
			return;
		}

		my_free_list = free_list + FREE_LIST_INDEX(n);
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}

	template <bool threads, int inst>
	void * __default_alloc_template<threads, inst>::refill(size_t n) {
		int nobjs = 20;
		char* chunk = chunk_alloc(n, nobjs);
		obj* volatile *my_free_list;
		obj* result;
		obj* current_obj, *next_obj;
		int i;

		if (1 == nobjs)
			return chunk;
		my_free_list = free_list + FREE_LIST_INDEX(n);

		result = reinterpret_cast<obj*>(chunk);
		*my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n);
		for (i = 1; ; ++i) {
			current_obj = next_obj;
			next_obj = reinterpret_cast<obj*>(reinterpret_cast<char*>(next_obj) + n);
			if (nobjs - 1 == i) {
				current_obj->free_list_link = nullptr;
				break;
			}
			else {
				current_obj->free_list_link = next_obj;
			}
		}
		return result;
	}

	template <bool threads, int inst>
	char * __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs) {
		char* result;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) {
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else if (bytes_left > size) {
			nobjs = static_cast<int>(bytes_left / size);
			total_bytes = size * nobjs;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {
				obj* volatile *my_free_list = free_list + FREE_LIST_INDEX(bytes_left);
				(reinterpret_cast<obj*>(start_free))->free_list_link = *my_free_list;
				*my_free_list = reinterpret_cast<obj*>(start_free);
			}

			start_free = reinterpret_cast<char*>(malloc(bytes_to_get));
			if (start_free == nullptr) {
				obj* volatile *my_free_list, *p;
				for (size_t i = size; i <= static_cast<size_t>(_MAX_BYTES); i += static_cast<size_t>(_ALIGN)) {
					my_free_list = free_list + FREE_LIST_INDEX(i);
					p = *my_free_list;
					if (p != nullptr) {
						*my_free_list = p->free_list_link;
						start_free = reinterpret_cast<char*>(p);
						end_free = start_free + i;
						return chunk_alloc(size, nobjs);
					}
				}
				end_free = nullptr;
				start_free = reinterpret_cast<char*>(__malloc_alloc_template<0>::allocate(bytes_to_get));
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(size, nobjs);
		}
	}

	template <bool threads, int inst>
	void * __default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
		void* result;
		size_t copy_sz;

		if (old_sz > static_cast<size_t>(_MAX_BYTES) && new_sz > static_cast<size_t>(_MAX_BYTES))
			return realloc(p, new_sz);
		if (ROUND_UP(old_sz) == ROUND_UP(new_sz))
			return p;
		result = allocate(new_sz);
		copy_sz = old_sz > new_sz ? old_sz : new_sz;
		memcpy(result, p, copy_sz);
		deallocate(p, old_sz);
		return result;
	}
}
