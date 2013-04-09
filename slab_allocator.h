#ifndef __SLAB_ALLOCATOR_H__
#define __SLAB_ALLOCATOR_H__

#include <stdlib.h>
#include <assert.h>

#include "aligned_allocator.h"
#include "parameters.h"
#include "container.h"

class slab_allocator
{
public:
	class linked_obj {
		linked_obj *next;
	public:
		linked_obj() {
			next = NULL;
		}

		void add(linked_obj *obj) {
			obj->next = this->next;
			this->next = obj;
		}

		linked_obj *get_next() {
			return next;
		}

		void set_next(linked_obj *obj) {
			next = obj;
		}
	};

	class linked_obj_list {
		linked_obj head;
		linked_obj *end;		// point to the last element or NULL.
		int size;
	public:
		linked_obj_list() {
			size = 0;
			end = NULL;
		}

		void add(linked_obj *obj) {
			if (get_size() == 0) {
				end = obj;
				assert(head.get_next() == NULL);
			}
			head.add(obj);
			size++;
		}

		void add_list(linked_obj_list *list) {
			if (list->get_size() == 0)
				return;
			if (get_size() == 0) {
				size = list->get_size();
				head = list->head;
				end = list->end;
				list->head.set_next(NULL);
				list->end = NULL;
				list->size = 0;
			}
			else {
				list->end->set_next(head.get_next());
				head = list->head;
				size += list->size;
				list->head.set_next(NULL);
				list->end = NULL;
				list->size = 0;
			}
		}

		/**
		 * It removes the specified number of objects from the list
		 * and returns them in a form of linked list..
		 */
		linked_obj *pop(int num) {
			linked_obj *obj = head.get_next();
			int num_pop = 0;
			linked_obj *prev_obj = NULL;
			for (int i = 0; i < num && obj != NULL; i++) {
				prev_obj = obj;
				obj = obj->get_next();
				num_pop++;
			}
			linked_obj *ret = head.get_next();
			if (prev_obj)
				prev_obj->set_next(NULL);

			head.set_next(obj);
			size -= num_pop;
			assert(size >= 0);
			if (size == 0)
				end = NULL;
			return ret;
		}

		int get_size() {
			return size;
		}
	};

private:
	const int obj_size;
	// the size to increase each time there aren't enough objects
	const long increase_size;
	// the maximal size of all objects
	const long max_size;
	const int node_id;

	linked_obj_list list;
	// the current size of memory used by the allocator.
	long curr_size;

	pthread_spinlock_t lock;

#ifdef MEMCHECK
	aligned_allocator allocator;
#endif
public:
	slab_allocator(int _obj_size, long _increase_size, long _max_size,
			int _node_id = -1): obj_size(_obj_size), increase_size(
				_increase_size), max_size(_max_size), node_id(_node_id)
#ifdef MEMCHECK
		, allocator(obj_size)
#endif
	{
		curr_size = 0;
		assert((unsigned) obj_size >= sizeof(linked_obj));
		pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
	}

	virtual ~slab_allocator();

	int alloc(char **objs, int num);

	void free(char **objs, int num);

	long get_max_size() const {
		return max_size;
	}
};

const static long MAX_SIZE = 0x7fffffffffffffffL;

template<class T>
class obj_allocator: public slab_allocator
{
	// The buffers pre-allocated to serve allocation requests
	// from the local threads.
	pthread_key_t local_buf_key;
	// The buffers freed in the local threads, which hasn't been
	// added the main buffer.
	pthread_key_t local_free_key;
public:
	obj_allocator(long increase_size,
			long max_size = MAX_SIZE): slab_allocator(sizeof(T),
				increase_size, max_size) {
		assert(increase_size <= max_size);
		pthread_key_create(&local_buf_key, NULL);
		pthread_key_create(&local_free_key, NULL);
	}

	int alloc_objs(T **objs, int num) {
		int ret = slab_allocator::alloc((char **) objs, num);
		T obj;
		for (int i = 0; i < ret; i++) {
			*objs[i] = obj;
		}
		return ret;
	}

	T *alloc_obj() {
		fifo_queue<T *> *local_buf_refs
			= (fifo_queue<T *> *) pthread_getspecific(local_buf_key);
		if (local_buf_refs == NULL) {
			local_buf_refs = new fifo_queue<T *>(LOCAL_BUF_SIZE);
			pthread_setspecific(local_buf_key, local_buf_refs);
		}

		if (local_buf_refs->is_empty()) {
			T *objs[LOCAL_BUF_SIZE];
			int num = alloc_objs(objs, LOCAL_BUF_SIZE);
			assert(num > 0);
			int num_added = local_buf_refs->add(objs, num);
			assert(num_added == num);
		}
		return local_buf_refs->pop_front();
	}

	void free(T **objs, int num) {
		slab_allocator::free((char **) objs, num);
	}

	void free(T *obj) {
		fifo_queue<T *> *local_free_refs
			= (fifo_queue<T *> *) pthread_getspecific(local_free_key);
		if (local_free_refs == NULL) {
			local_free_refs = new fifo_queue<T *>(LOCAL_BUF_SIZE);
			pthread_setspecific(local_free_key, local_free_refs);
		}
		if (local_free_refs->is_full()) {
			T *objs[LOCAL_BUF_SIZE];
			int num = local_free_refs->fetch(objs, LOCAL_BUF_SIZE);
			free(objs, num);
		}
		local_free_refs->push_back(obj);
	}
};

#endif
