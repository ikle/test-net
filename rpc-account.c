/*
 * RPC Accounting
 *
 * Copyright (c) 2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <fcntl.h>
#include <unistd.h>
#include <utmpx.h>

#include "rpc-account.h"

int rpc_pty_open (void)
{
	int dev, fd;
	char path[36];  /* never overflows for 64-bit ints */

	mkdir ("/var/run/rpc", 0777);

	for (dev = 0; dev < INT_MAX; ++dev) {
		snprintf (path, sizeof (path), "/var/run/rpc/%d", dev);

		if ((fd = open (path, O_RDWR | O_CREAT | O_EXCL, 0600)) != -1) {
			close (fd);
			return dev;
		}

		if (errno != EEXIST)
			return -1;
	}

	errno = ENODEV;
	return -1;
}

int rpc_pty_close (int dev)
{
	char path[36];  /* never overflows for 64-bit ints */

	snprintf (path, sizeof (path), "/var/run/rpc/%d", dev);

	return unlink (path) == 0;
}

static void utmpx_init (struct utmpx *ut, int dev, int type)
{
	memset (ut, 0, sizeof (*ut));

	ut->ut_type = type;

	snprintf (ut->ut_line, sizeof (ut->ut_line), "rpc/%d", dev);
	snprintf (ut->ut_id,   sizeof (ut->ut_id),   "R%d",    dev);
}

int rpc_pty_login (int dev, const char *user, const char *host)
{
	struct utmpx ut;
	struct timeval tv;
	int ok;

	utmpx_init (&ut, dev, USER_PROCESS);
	ut.ut_pid = getpid ();
	strncpy (ut.ut_user, user, sizeof (ut.ut_user));

	if (host != NULL)
		strncpy (ut.ut_host, host, sizeof (ut.ut_host));

	gettimeofday (&tv, NULL);

	ut.ut_tv.tv_sec  = tv.tv_sec;
	ut.ut_tv.tv_usec = tv.tv_usec;

	setutxent ();
	ok = pututxline (&ut) != NULL;
	endutxent ();

	return ok;
}

int rpc_pty_logout (int dev)
{
	struct utmpx ut;
	int ok;

	utmpx_init (&ut, dev, DEAD_PROCESS);

	setutxent ();
	ok = pututxline (&ut) != NULL;
	endutxent ();

	return ok;
}
