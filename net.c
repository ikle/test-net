/*
 * Network socket helpers
 *
 * Copyright (c) 2006-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include "net.h"

#ifndef AI_IDN
#define AI_IDN  0
#endif

static int netdb_errno (int code)
{
	switch (code) {
	case EAI_BADFLAGS:
		return EINVAL;
	case EAI_NONAME:
#ifdef EAI_NODATA
	case EAI_NODATA:
#endif
		return EADDRNOTAVAIL;
	case EAI_AGAIN:
		return EAGAIN;
	case EAI_FAIL:
		return EPROTO;
	case EAI_FAMILY:
	case EAI_SOCKTYPE:
	case EAI_SERVICE:
#ifdef EAI_ADDRFAMILY
	case EAI_ADDRFAMILY:
#endif
		return EAFNOSUPPORT;
	case EAI_MEMORY:
		return ENOMEM;
	case EAI_SYSTEM:
		return errno;
	case EAI_OVERFLOW:
		return EOVERFLOW;
#ifdef EAI_INTR
	case EAI_INTR:
		return EINTR;
#endif
	}

	return EIO;
}

static int unix_connect (int type, const char *node)
{
	struct sockaddr_un sa;
	int s;

	if (strlen (node) >= sizeof (sa.sun_path))
		return -EINVAL;

	if ((s = socket (AF_UNIX, type, 0)) == -1)
		return -errno;

	strncpy (sa.sun_path, node, sizeof (sa.sun_path));

	if (connect (s, (void *) &sa, sizeof (sa)) != -1)
		return s;

	close (s);
	return -errno;
}

int net_connect (int type, const char *node, const char *service)
{
	struct addrinfo hints, *list, *p;
	int s;

	if (service == NULL)
		return unix_connect (type, node);

	memset (&hints, 0, sizeof (hints));

	hints.ai_family   = AF_INET;
	hints.ai_socktype = type;
	hints.ai_protocol = 0;
	hints.ai_flags    = AI_V4MAPPED | AI_ADDRCONFIG | AI_IDN;

	if ((s = getaddrinfo (node, service, &hints, &list)) != 0)
		return -netdb_errno (s);

	for (p = list; p != NULL; p = p->ai_next) {
		s = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
		if (s != -1)
			continue;

		if (connect (s, p->ai_addr, p->ai_addrlen) != -1)
			return s;

		close (s);
	}

	freeaddrinfo (list);
	return -errno;
}

int net_listen (int type, const char *node, const char *service)
{
	struct addrinfo hints, *list, *p;
	int s;

	memset (&hints, 0, sizeof (hints));

	hints.ai_family   = AF_INET;
	hints.ai_socktype = type;
	hints.ai_protocol = 0;
	hints.ai_flags    = AI_V4MAPPED | AI_ADDRCONFIG | AI_IDN | AI_PASSIVE;

	if ((s = getaddrinfo (node, service, &hints, &list)) != 0)
		return -netdb_errno (s);

	for (p = list; p != NULL; p = p->ai_next) {
		s = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
		if (s != -1)
			continue;

		if (bind (s, p->ai_addr, p->ai_addrlen) != -1 &&
		    listen (s, 10) != -1)
			return s;

		close (s);
	}

	freeaddrinfo (list);
	return -errno;
}

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
