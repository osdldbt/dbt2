/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

/* Using GNU extensions for pinning to processors. */
#define _GNU_SOURCE

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <ev.h>

#include "_socket.h"
#include "client.h"
#include "common.h"
#include "db.h"
#include "listener.h"
#include "logging.h"
#include "transaction_queue.h"

struct accept_io {
	struct ev_io io;
	int socket;
};

struct driver_io {
	struct ev_io io;
	struct db_context_t dbc;
};

struct recv_io {
	struct ev_io io;
	int socket;
	struct db_context_t dbc;
};

struct udsmsg {
	int fd;
	struct cmsghdr h;
};

/* Global Variables */
int id = -1;

int pcsock[2];

static struct accept_io io_uds;
static struct driver_io io_dds;

/*
 * Use an array indexed by the socket file descriptor (i.e. a number) for an
 * easy to manage data structure of the watchers for each driver connection.
 * There will be holes and we need to be sure to allocate a big enough array
 * for the expected number of connections.  e.g. 1 is stdout, 2 is stderr and
 * another process may have already have other sockets.  But hey, this is a
 * synthetic benchmark!
 */
static struct recv_io *io_vds;

/* Callback for when the client receives a transaction request from driver. */
static void
recv_data_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
	struct recv_io *ri = (struct recv_io *) watcher;

	int rc;
	int length;
	struct client_transaction_t client_data;

	rc = _receive(
			ri->socket, &client_data, sizeof(struct client_transaction_t));
	if (rc == ERROR_SOCKET_CLOSED) {
		LOG_ERROR_MESSAGE(
				"[%d] socket %d unexpectedly closed", getpid(), ri->socket);
		ev_io_stop(loop, watcher);
		return;
	} else if (rc == 0) {
		LOG_ERROR_MESSAGE(
				"[%d] socket %d unexpectedly received 0 data", getpid(),
				ri->socket);
		ev_io_stop(loop, watcher);
		return;
	} else if (rc == -1) {
		LOG_ERROR_MESSAGE(
				"[%d] receive_transaction_data() error, closing %d", getpid(),
				ri->socket);
		ev_io_stop(loop, watcher);
		close(ri->socket);
		return;
	}

	client_data.status = process_transaction(
			client_data.transaction, &ri->dbc, &client_data.transaction_data);
	if (client_data.status == ERROR) {
		printf("process_transaction() error on %s\n",
			   transaction_name[client_data.transaction]);
		return;
	}

	length =
			_send(ri->socket, (void *) &client_data,
				  sizeof(struct client_transaction_t));
	if (length == -1) {
		printf("[%d] send_transaction_data() error: %d\n", getpid(),
			   ri->socket);
	}
}

/* Callback when client child process receives a new driver connection. */
static void
recv_sock_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
	struct ev_loop *sub_loop = EV_DEFAULT;
	struct driver_io *ri = (struct driver_io *) watcher;

	int length;
	struct udsmsg buf;

	struct msghdr msghdr;
	char nothing;
	struct iovec nothing_ptr;
	struct cmsghdr *cmsg;

	int thissock;

	nothing_ptr.iov_base = &nothing;
	nothing_ptr.iov_len = 1;
	msghdr.msg_name = NULL;
	msghdr.msg_namelen = 0;
	msghdr.msg_iov = &nothing_ptr;
	msghdr.msg_iovlen = 1;
	msghdr.msg_flags = 0;
	msghdr.msg_control = &buf;
	msghdr.msg_controllen = sizeof(struct cmsghdr) + sizeof(int);
	cmsg = CMSG_FIRSTHDR(&msghdr);
	cmsg->cmsg_len = msghdr.msg_controllen;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	((int *) CMSG_DATA(cmsg))[0] = -1;

	length = recvmsg(pcsock[1], &msghdr, 0);
	if (length == -1) {
		LOG_ERROR_MESSAGE(
				"[%d] recvmsg() error waiting for socket\n", getpid());
		return;
	}
	thissock = ((int *) CMSG_DATA(cmsg))[0];

	/* Create new watcher on the new connection. */
	if (io_vds[thissock - 1].socket != 0) {
		memset(&io_vds[thissock - 1], 0, sizeof(struct recv_io));
	}
	io_vds[thissock - 1].socket = thissock;
	io_vds[thissock - 1].dbc = ri->dbc;
	ev_io_init(
			(struct ev_io *) &io_vds[thissock - 1], recv_data_cb, thissock,
			EV_READ);
	ev_io_start(sub_loop, (struct ev_io *) &io_vds[thissock - 1]);
}

/* Callback for when the client has to accept a connection from the driver. */
static void
accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
	int newsfd;
	struct udsmsg buf;

	struct msghdr msghdr;
	char nothing = ' ';
	struct iovec nothing_ptr;
	struct cmsghdr *cmsg;

	newsfd = _accept(&watcher->fd);
	if (newsfd == -1) {
		printf("_accept() failed, trying again...");
		return;
	}

	/*
	 * Queue up the socket by sending it to the first child process that
	 * picks up the message.
	 */
	nothing_ptr.iov_base = &nothing;
	nothing_ptr.iov_len = 1;
	msghdr.msg_name = NULL;
	msghdr.msg_namelen = 0;
	msghdr.msg_iov = &nothing_ptr;
	msghdr.msg_iovlen = 1;
	msghdr.msg_flags = 0;
	msghdr.msg_control = &buf;
	msghdr.msg_controllen = sizeof(struct cmsghdr) + sizeof(int);
	cmsg = CMSG_FIRSTHDR(&msghdr);
	cmsg->cmsg_len = msghdr.msg_controllen;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	((int *) CMSG_DATA(cmsg))[0] = newsfd;
	sendmsg(pcsock[0], &msghdr, 0);
}

/* The child processes do all the work. */
int loop_child(int id) {
	struct ev_loop *sub_loop = EV_DEFAULT;

	io_vds = malloc(sizeof(struct recv_io) * max_driver_connections);
	if (io_vds == NULL) {
		printf("[%d] error allocating memory of driver connection\n", getpid());
		return 0;
	}
	memset(io_vds, 0, sizeof(struct recv_io) * max_driver_connections);

	if (init_dbc(&io_dds.dbc) != 0) {
		return 1;
	}

	if (!exiting && connect_to_db(&io_dds.dbc) != OK) {
		printf("cannot connect to database, exiting...\n");
		exit(1);
	}
	printf("[%d] connected to database\n", getpid());
	fflush(stdout);

	ev_io_init((struct ev_io *) &io_dds, recv_sock_cb, pcsock[1], EV_READ);
	ev_io_start(sub_loop, (struct ev_io *) &io_dds);

	ev_run(sub_loop, 0);

	/* Disconnect from the database. */
	disconnect_from_db(&io_dds.dbc);
	free(io_vds);
	return 0;
}

/* Use the parent process just to accept connections from the driver. */
int loop_parent() {
	struct ev_loop *main_loop = EV_DEFAULT;

	memset(&io_uds, 0, sizeof(struct accept_io));
	ev_io_init((struct ev_io *) &io_uds, accept_cb, sockfd, EV_READ);
	io_uds.socket = sockfd;
	ev_io_start(main_loop, (struct ev_io *) &io_uds);

	ev_run(main_loop, 0);

	return 0;
}

int startup() {
	int i;
	const int nprocs = get_nprocs();
	const int connections = nprocs * connections_per_process;
	cpu_set_t set;
	extern char sname[SNAMELEN + 1];

	/* Use a unix domain socket as a queue for incoming driver connections. */
	if (socketpair(PF_UNIX, SOCK_STREAM, 0, pcsock)) {
		perror("socketpair");
		return 99;
	}

	printf("this system has %d out of %d processors available.\n", nprocs,
		   get_nprocs_conf());

	CPU_ZERO(&set);

	printf("opening %d connection(s) to %s...\n", connections, sname);

	/* Spawn a child process for each configured processor. */
	for (i = 0; i < connections; i++) {
		int rc = fork();

		if (rc == 0) {
			int cpu = i % nprocs;

			id = i;

			CPU_SET(cpu, &set);
			if (sched_setaffinity(0, sizeof(set), &set) == -1) {
				perror("sched_setaffinity");
				return 1;
			}
			printf("[%d] pinning to processor %d.\n", getpid(), cpu);
			break;
		} else if (rc == -1) {
			perror("fork");
			return 1;
		} else {
			continue;
		}
	}

	init_logging_f();
	free(output_path);

	if (id >= 0) {
		if (loop_child(id) != 0) {
			printf("[%d] problem starting child loop\n", getpid());
			return 1;
		}
		printf("[%d] child exiting\n", getpid());
		return 0;
	}

	loop_parent();
	return 0;
}

void status() { printf("not implemented\n"); }
