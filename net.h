/*
 * Network socket helpers
 *
 * Copyright (c) 2006-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _IKLE_NET_H
#define _IKLE_NET_H 1

#include <sys/types.h>
#include <sys/socket.h>

/*
 * The function net_connect() create the socket and connects it to the
 * address specified by node:service pair. If node is NULL, then the network
 * address will be set to the loopback interface address.
 *
 * If the node/servise name resolution, socket creation and connection
 * succeeds, socket is returned. On error negative error code is returned.
 */
int net_connect (int type, const char *node, const char *service);

/*
 * The function net_listen() create the socket, bind it and mark a socket as
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
int net_listen (int type, const char *node, const char *service);

/*
 * The function net_set_blocking sets file to blocking or non-blocking mode.
 *
 * On success non-zero is returned. On error zero is returned and errno is
 * set appropriately.
 */
int net_set_blocking (int fd, int blocking);

#endif /* _IKLE_NET_H */
