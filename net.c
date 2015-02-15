/*
 * Copyright (c) 2006, 2010, 2015 Alexei A. Smekalkine <ikle at ikle.ru>.
 * All rights reserved.
 *
 * This program is free software.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE DEVELOPERS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "net.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#ifndef AI_IDN
#define AI_IDN  0
#endif

static int netdb_errno (int code)
{
	switch (code) {
	case EAI_BADFLAGS:
		return EINVAL;
	case EAI_NONAME:
		return EADDRNOTAVAIL;
	case EAI_AGAIN:
		return EAGAIN;
	case EAI_FAIL:
		return EPROTO;
	case EAI_FAMILY:
	case EAI_SERVICE:
		return EAFNOSUPPORT;
	case EAI_MEMORY:
		return ENOMEM;
	case EAI_SYSTEM:
		return errno;
	case EAI_OVERFLOW:
		return EOVERFLOW;
	}

	return EIO;
}

/*
 * The function net_socket() translates the node and/or the service name,
 * and, on success, create the socket for the first found record.
 *
 * If passive in not zero and node is NULL, then the returned socket address
 * will contain the "wilcard address" suitable for bind(2)ing a socket that
 * will accept(2) connections.
 *
 * If passive is zero and node is NULL, then the network address will be set
 * to the loopback interface address.
 *
 * If the node/servise name resolution and socket creation succeeds, socket
 * and found resource list is returned. On error negative error code is
 * returned.
 *
 * Use freeaddrinfo() function to free returned resource list.
 */
static
int net_socket (int type, const char *node, const char *service,
		struct addrinfo **res, int passive)
{
	struct addrinfo hints;
	int ret;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = type;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG | AI_IDN;
	if (passive)
		hints.ai_flags |= AI_PASSIVE;

	if ((ret = getaddrinfo (node, service, &hints, res)) != 0)
		return -netdb_errno (ret);

	/* using first resource record element... */
	ret = socket ((*res)->ai_family, (*res)->ai_socktype,
		      (*res)->ai_protocol);
	if (ret == -1)
		goto no_socket;

	return ret;
no_socket:
	freeaddrinfo (*res);
	*res = NULL;
	return -errno;
}

/*
 * The function net_connect() create the socket and connects it to the
 * address specified by node:service pair. If node is NULL, then the network
 * address will be set to the loopback interface address.
 *
 * If the node/servise name resolution, socket creation and connection
 * succeeds, socket is returned. On error, negative error code is returned.
 */
int net_connect (int type, const char *node, const char *service)
{
	struct addrinfo *res;
	int s, saved_errno;

	/* using first resource record element... */
	if ((s = net_socket (type, node, service, &res, 0)) < 0)
		return s;

	if (connect (s, res->ai_addr, res->ai_addrlen) == -1) {
		saved_errno = errno;
		freeaddrinfo (res);
		close (s);
		return -saved_errno;
	}

	freeaddrinfo (res);
	return s;
}

/*
 * The function net_socket() create the socket, bind it and mark a socket as
 * passive, that is, as a socket that will be used to accept incoming
 * connection request using accept(2).
 *
 * If node is not NULL, then function assigns the address specified by node
 * to a created socket, overwise to the "wilcard address".
 *
 * If the node/servise name resolution, socket creation, bind(2)ing and
 * linten(2)ing succeeds, socket is returned. On error negative error code
 * is returned.
 */
int net_listen (int type, const char *node, const char *service)
{
	struct addrinfo *res;
	int s, saved_errno;

	/* using first resource record element... */
	if ((s = net_socket (type, node, service, &res, 1)) < 0)
		return s;

	if (bind (s, res->ai_addr, res->ai_addrlen) == -1 ||
	    listen (s, 10) == -1) {
		saved_errno = errno;
		freeaddrinfo (res);
		close (s);
		return -saved_errno;
	}

	freeaddrinfo (res);
	return s;
}

/*
 * The function net_set_blocking sets file to blocking or non-blocking mode.
 *
 * On success non-zero is returned. On error zero is returned and errno is
 * set appropriately.
 */
int net_set_blocking (int fd, int blocking)
{
	int flags;

	if ((flags = fcntl (fd, F_GETFL)) == -1)
		return 0;

	if (blocking)
		flags &= ~O_NONBLOCK;
	else
		flags |=  O_NONBLOCK;

	return fcntl (fd, F_SETFL, flags) != -1;
}
