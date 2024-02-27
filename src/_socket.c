/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright The DBT-2 Authors
 */

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "_socket.h"

#ifndef INADDR_NONE
#define INADDR_NONE -1
#endif

int resolveproto(const char *proto);

int _accept(int *s) {
	socklen_t addrlen;
	struct sockaddr_in sa;
	int sockfd;

	addrlen = sizeof(struct sockaddr_in);
	sockfd = accept(*s, (struct sockaddr *) &sa, &addrlen);
	if (sockfd == -1) {
		perror("_accept");
		printf("Can't accept driver connection, possible reasons may be\n");
		printf("that the number of allowed open files may be exceeded.\n");
		printf("This could be limited by the operating system or by the\n");
		printf("user's security limits.\n");
	}
	return sockfd;
}

int _connect(char *address, unsigned short port) {
	int sockfd = -1;
	struct addrinfo hints;
	struct addrinfo *result = NULL, *rp;
	int s;
	char port_str[8];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	snprintf(port_str, sizeof(port_str), "%d", port);

	s = getaddrinfo(address, port_str, &hints, &result);
	if (s != 0) {
		close(sockfd);
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1) {
			continue;
		}

		if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}

		close(sockfd);
		sockfd = -1;
	}

	if (sockfd == -1) {
		printf("Can't create socket for connection to client\n");
	}

	freeaddrinfo(result);
	if (rp == NULL) {
		fprintf(stderr, "Could not connect\n");
		return -1;
	}

	return sockfd;
}

int _receive(int s, void *data, int length) {
	int received, total, remaining;
	remaining = length;
	total = 0;
	do {
		received = recv(s, data, remaining, 0);
		if (received == -1) {
			perror("recv");
			return -1;
		} else if (received == 0) {
			return ERR_SOCKET_CLOSED;
		}
		total += received;
		data += received;
		remaining -= received;
	} while (total != length);
	return total;
}

int _send(int s, void *data, int length) {
	int sent = 0;
	int remaining = length;
	do {
		sent = send(s, (void *) data, remaining, 0);
		if (sent == -1) {
			return -1;
		} else if (sent == 0) {
			return 0;
		}
		data += sent;
		remaining -= sent;
	} while (sent != length);
	return sent;
}

int _listen(int port) {
	struct sockaddr_in sa;
	int val;
	int sockfd;

	val = 1;

	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons((unsigned short) port);

	sockfd = socket(PF_INET, SOCK_STREAM, resolveproto("TCP"));
	if (sockfd < 0) {
		perror("_listen");
		return ERR_SOCKET_CREATE;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if (bind(sockfd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0) {
		perror("_listen");
		return ERR_SOCKET_BIND;
	}

	if (listen(sockfd, 1) < 0) {
		perror("_listen");
		return ERR_SOCKET_LISTEN;
	}
	return sockfd;
}

int resolveproto(const char *proto) {
	struct protoent *protocol;

	protocol = getprotobyname(proto);
	if (!protocol) {
		return ERR_SOCKET_RESOLVPROTO;
	}

	return protocol->p_proto;
}
