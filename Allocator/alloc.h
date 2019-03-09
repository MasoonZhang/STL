#pragma once

#include <new>
#include <string>
#include <cstdlib>

namespace STL {

	template<int inst>
	class _malloc_alloc_template {

	private:
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void(*_malloc_alloc_oom_handler)();

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
			void (*old)() = _malloc_alloc_oom_handler;
			_malloc_alloc_oom_handler = f;
			return old;
		}

		void(*_malloc_alloc_oom_handler)() = nullptr;

		void* oom_malloc(size_t n) {
			void(*my_malloc_handler)();
			void* result;

			for (;;) {
				my_malloc_handler = _malloc_alloc_oom_handler;
				if (my_malloc_handler == nullptr)
					throw std::bad_alloc();
				(*my_malloc_handler)();
				result = malloc(n);
				if (result)
					return result;
			}
		}

		void* oom_realloc(void* p, size_t n) {
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
	};

	

	template<bool threads, int inst>
	class _default_alloc_template {

	private:
		enum { _ALIGN = 8, _MAX_BYTES = 128, _NFREELISTS = _MAX_BYTES / _ALIGN };
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

		char* start_free = nullptr;
		char* end_free = nullptr;
		size_t heap_size = 0;
		obj* volatile free_list[_NFREELISTS] = {
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
		};

		void* allocate(size_t n) {
			obj* volatile *my_free_list;
			obj* result;
			if (n > static_cast<size_t>(_MAX_BYTES)) {
				return _malloc_alloc_template<0>::allocate(n);
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

		void deallocate(void* p, size_t n) {
			obj* q = reinterpret_cast<obj *>(p);
			obj* volatile *my_free_list;

			if (n > (size_t)_MAX_BYTES) {
				_malloc_alloc_template<0>::deallocate(p, n);
				return;
			}

			my_free_list = free_list + FREE_LIST_INDEX(n);
			q->free_list_link = *my_free_list;
			*my_free_list = q;
		}

		void* refill(size_t n) {
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

		char* chunk_alloc(size_t size, int& nobjs) {
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
					start_free = reinterpret_cast<char*>(_malloc_alloc_template<0>::allocate(bytes_to_get));
				}
				heap_size += bytes_to_get;
				end_free = start_free + bytes_to_get;
				return chunk_alloc(size, nobjs);
			}
		}
		void* reallocate(void* p, size_t old_sz, size_t new_sz) {
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
	};
}
