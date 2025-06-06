/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>
#include <err_utils.h>

/*
 * This example was shamelessly stoen from:
 * https://unixism.net/loti/tutorial/cp_liburing.html
 * It shows how to use the io_uring library API to implement a cat(1) like program
 *
 * EXTRA_LINK_FLAGS_AFTER=-luring
 */

#define QD 2
#define BS (16 * 1024)

static int infd, outfd;

struct io_data{
	int read;
	off_t first_offset, offset;
	size_t first_len;
	struct iovec iov;
};

static int setup_context(unsigned entries, struct io_uring *ring) {
	int ret;

	ret = io_uring_queue_init(entries, ring, 0);
	if( ret < 0) {
		fprintf(stderr, "queue_init: %s\n", strerror(-ret));
		return -1;
	}
	return 0;
}

static int get_file_size(int fd, off_t *size) {
	struct stat st;
	if (fstat(fd, &st) < 0 )
		return -1;
	if(S_ISREG(st.st_mode)) {
		*size = st.st_size;
		return 0;
	} else if (S_ISBLK(st.st_mode)) {
		unsigned long long bytes;
		if (ioctl(fd, BLKGETSIZE64, &bytes) != 0)
			return -1;
		*size = bytes;
		return 0;
	}
	return -1;
}

static void queue_prepped(struct io_uring *ring, struct io_data *data) {
	struct io_uring_sqe *sqe;

	sqe = io_uring_get_sqe(ring);
	assert(sqe);
	if (data->read)
		io_uring_prep_readv(sqe, infd, &data->iov, 1, data->offset);
	else
		io_uring_prep_writev(sqe, outfd, &data->iov, 1, data->offset);
	io_uring_sqe_set_data(sqe, data);
}

static int queue_read(struct io_uring *ring, off_t size, off_t offset) {
	struct io_uring_sqe *sqe;
	struct io_data *data;

	data = (io_data*)malloc(size + sizeof(*data));
	if (!data)
		return 1;
	sqe = io_uring_get_sqe(ring);
	if (!sqe) {
		free(data);
		return 1;
	}
	data->read = 1;
	data->offset = data->first_offset = offset;

	data->iov.iov_base = data + 1;
	data->iov.iov_len = size;
	data->first_len = size;

	io_uring_prep_readv(sqe, infd, &data->iov, 1, offset);
	io_uring_sqe_set_data(sqe, data);
	return 0;
}

static void queue_write(struct io_uring *ring, struct io_data *data) {
	data->read = 0;
	data->offset = data->first_offset;

	data->iov.iov_base = data + 1;
	data->iov.iov_len = data->first_len;

	queue_prepped(ring, data);
	io_uring_submit(ring);
}

int copy_file(struct io_uring *ring, off_t insize) {
	unsigned long reads, writes;
	struct io_uring_cqe *cqe;
	off_t write_left, offset;
	int ret;

	write_left = insize;
	writes = reads = offset = 0;
	while (insize || write_left) {
		unsigned long had_reads, got_comp;

		/* Queue up as many reads as we can */
		had_reads = reads;
		while (insize) {
			off_t this_size = insize;
			if (reads + writes >= QD)
				break;
			if (this_size > BS)
				this_size = BS;
			else if (!this_size)
				break;
			if (queue_read(ring, this_size, offset))
				break;
			insize -= this_size;
			offset += this_size;
			reads++;
		}
		if (had_reads != reads) {
			ret = io_uring_submit(ring);
			if (ret < 0) {
				fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
				break;
			}
		}
		/* Queue is full at this point. Let's find at least one completion */
		got_comp = 0;
		while (write_left) {
			struct io_data *data;
			if (!got_comp) {
				ret = io_uring_wait_cqe(ring, &cqe);
				got_comp = 1;
			} else {
				ret = io_uring_peek_cqe(ring, &cqe);
				if (ret == -EAGAIN) {
					cqe = NULL;
					ret = 0;
				}
			}
			if (ret < 0) {
				fprintf(stderr, "io_uring_peek_cqe: %s\n",
					strerror(-ret));
				return 1;
			}
			if (!cqe)
				break;
			data = (io_data*)io_uring_cqe_get_data(cqe);
			if (cqe->res < 0) {
				if (cqe->res == -EAGAIN) {
					queue_prepped(ring, data);
					io_uring_cqe_seen(ring, cqe);
					continue;
				}
				fprintf(stderr, "cqe failed: %s\n",
					strerror(-cqe->res));
				return 1;
			} else if (cqe->res != (int)data->iov.iov_len) {
				/* short read/write; adjust and requeue */
				data->iov.iov_base = (char*)(data->iov.iov_base)+cqe->res;
				data->iov.iov_len -= cqe->res;
				queue_prepped(ring, data);
				io_uring_cqe_seen(ring, cqe);
				continue;
			}
			/*
			 * All done. If write, nothing else to do. If read,
			 * queue up corresponding write.
			 * */
			if (data->read) {
				queue_write(ring, data);
				write_left -= data->first_len;
				reads--;
				writes++;
			} else {
				free(data);
				writes--;
			}
			io_uring_cqe_seen(ring, cqe);
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	struct io_uring ring;
	off_t insize;
	int ret;
	if (argc < 3) {
		printf("%s: usage: %s <infile> <outfile>\n", argv[0], argv[0]);
		return 1;
	}
	CHECK_NOT_M1(infd = open(argv[1], O_RDONLY));

	CHECK_NOT_M1(outfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644));
	if (setup_context(QD, &ring))
		return 1;
	if (get_file_size(infd, &insize))
		return 1;
	ret = copy_file(&ring, insize);

	close(infd);
	close(outfd);
	io_uring_queue_exit(&ring);
	return ret;
}
