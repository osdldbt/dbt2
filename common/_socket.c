/*
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Labs, Inc.
 *
 * 20 March 2002
 */

#include <string.h>
#include <unistd.h>
#include <_socket.h>

int resolveproto(const char *proto);

int _accept(int *s)
{
	socklen_t addrlen;
	struct sockaddr_in sa;
	int sockfd;

	sockfd = accept(*s, (struct sockaddr *) &sa, &addrlen);
	return sockfd;
}

int _connect(char *address, unsigned short port) {
	int sockfd = -1;
	struct sockaddr_in sa;
	struct hostent *he;
	in_addr_t addr;

	bzero(&sa, sizeof(struct sockaddr_in));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (sa.sin_port == 0) {
		return -1;
	}

	/* Assume that an IP address is used. */
	addr = inet_addr(address);
	if (addr == INADDR_NONE) {
		/* If it is not an IP address, assume it is a hostname. */
		if ((he = gethostbyname(address)) == NULL) {
			close(sockfd);
			return -1;
		}
	}
	else
	{
		/* Continue the assumption that an IP address is used. */
		if ((he = gethostbyaddr((char *) (&addr), sizeof(addr),
			AF_INET)) == NULL) {
			close(sockfd);
			return -1;
		}
		memcpy(&sa.sin_addr, &addr, sizeof(addr));
	}
	memcpy(&sa.sin_addr, he->h_addr_list[0], he->h_length);

	sockfd = socket(PF_INET, SOCK_STREAM, resolveproto("tcp"));
	if (sockfd == -1) {
		return sockfd;
	}

	if (connect(sockfd, (struct sockaddr *) &sa,
		sizeof(struct sockaddr_in)) == -1) {
		return -1;
	}

	return sockfd;
}

int _receive(int s, void *data, int length)
{
	int received, total, remaining;
	remaining = length;
	total = 0;
	do {
		received = recv(s, data, remaining, 0);
		if (received == -1) {
			return -1;
		} else if (received == 0) {
			return 0;
		}
		total += received;
		data += received;
		remaining -= received;
	}
	while (total != length);
	return total;
}

int _send(int s, void *data, int length)
{
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
	}
	while (sent != length);
	return sent;
}

int _listen(int port)
{
	struct sockaddr_in sa;
	int sockfd;

	bzero(&sa, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons((unsigned short) port);
	sockfd = socket(PF_INET, SOCK_STREAM, resolveproto("TCP"));
	if (sockfd < 0) {
		return ERR_SOCKET_CREATE;
	}

	if (bind(sockfd, (struct sockaddr *)&sa,
		sizeof(struct sockaddr_in)) < 0) {
		return ERR_SOCKET_BIND;
	}

	if (listen(sockfd, 1) < 0) {
		return ERR_SOCKET_LISTEN;
	}
	return sockfd;
}

int resolveproto(const char *proto)
{
	struct protoent *protocol;

	protocol = getprotobyname(proto);
	if (!protocol) {
		return ERR_SOCKET_RESOLVPROTO;
	}

	return protocol->p_proto;
}
