/*
 * client_interface.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Open Source Development Lab, Inc.
 *
 * 5 august 2002
 */

#include <common.h>
#include <client_interface.h>

int connect_to_client(char *addr, int port)
{
	int sockfd;
	return sockfd;
}

int receive_transaction_data(int s, struct client_transaction_t *client_data)
{
	int rc;

	rc = _receive(s, client_data, sizeof(struct client_transaction_t));
	if (rc == -1)
	{
		LOG_ERROR_MESSAGE("cannot receive interaction data");
		return ERROR;
	}
	else if (rc == 0)
	{
		LOG_ERROR_MESSAGE("socket closed on _receive");
		return ERROR_SOCKET_CLOSED;
	}

	return OK;
}

int send_transaction_data(int s, struct client_transaction_t *client_data)
{
	if (_send(s, (void *) client_data,
		sizeof(struct client_transaction_t)) == -1)
	{
		LOG_ERROR_MESSAGE("cannot send transaction data");
		return ERROR;
	}

	return OK;
}
