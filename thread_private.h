#ifndef __THREAD_PRIVATE_H__
#define __THREAD_PRIVATE_H__

#include "workload.h"
#include "rand_buf.h"
#include "io_interface.h"
#include "thread.h"
#include "config.h"

class cleanup_callback;

/* this data structure stores the thread-private info. */
class thread_private: public thread
{
#ifdef USE_PROCESS
	pid_t id;
#else
	pthread_t id;
#endif
	/* the location in the thread descriptor array. */
	int idx;
	int node_id;
	workload_gen *gen;
	rand_buf *buf;
	io_interface *io;
	file_io_factory *factory;

	ssize_t read_bytes;
	long num_accesses;

	/* compute average number of pending IOs. */
	long tot_num_pending;
	long num_sampling;
	int max_num_pending;

	cleanup_callback *cb;
	
	struct timeval start_time, end_time;

#ifdef STATISTICS
public:
	atomic_integer num_completes;
	atomic_integer num_pending;
#endif

public:
	~thread_private() {
		if (buf)
			delete buf;
	}

	void init();

	int get_node_id() {
		return node_id;
	}

	thread_private(int node_id, int idx, int entry_size,
			file_io_factory *factory, workload_gen *gen): thread(
				std::string("test_thread") + itoa(idx), node_id) {
		this->cb = NULL;
		this->node_id = node_id;
		this->idx = idx;
		buf = NULL;
		this->gen = gen;
		this->io = NULL;
		this->factory = factory;
		read_bytes = 0;
		num_accesses = 0;
		num_sampling = 0;
		tot_num_pending = 0;
		max_num_pending = 0;
	}

	int attach2cpu();

	void run();

	ssize_t get_read_bytes();

	void print_stat();
};

#endif
