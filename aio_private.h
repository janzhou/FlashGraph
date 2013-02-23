#ifndef __AIO_PRIVATE_H__
#define __AIO_PRIVATE_H__

#include <deque>

#include "wpaio.h"
#include "read_private.h"

#ifdef ENABLE_AIO

void aio_callback(io_context_t, struct iocb*, void *, long, long);

struct thread_callback_s;

class async_io: public buffered_io
{
	int buf_idx;
	struct aio_ctx *ctx;
	std::deque<thread_callback_s *> cbs;
	callback *cb;
	const int AIO_DEPTH;
	// This is for allocating memory in the case that the memory given
	// by the user isn't in the local NUMA node.
	slab_allocator allocator;

	struct iocb *construct_req(io_request &io_req, callback_t cb_func);
public:
	/**
	 * @names: the names of files to be accessed
	 * @num: the number of files
	 * @size: the size of data to be accessed in all files
	 * @aio_depth_per_file
	 * @node_id: the NUMA node where the disks to be read are connected to.
	 */
	async_io(const char *names[], int num, long size, int aio_depth_per_file,
			int node_id);

	virtual ~async_io();

	ssize_t access(char *buf, off_t offset, ssize_t size, int access_method) {
		return -1;
	}

	ssize_t access(io_request *requests, int num);

	bool set_callback(callback *cb) {
		this->cb = cb;
		return true;
	}

	callback *get_callback() {
		return cb;
	}

	bool support_aio() {
		return true;
	}

	virtual void cleanup();

	void return_cb(thread_callback_s *tcb);

	int num_pending_IOs() const {
		return AIO_DEPTH - max_io_slot(ctx);
	}

	void wait4complete() {
		io_wait(ctx, NULL, 1);
	}
};
#endif

#endif
