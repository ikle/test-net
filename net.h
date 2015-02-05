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

#endif /* _IKLE_NET_H */
