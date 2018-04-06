/*
 * Packet Queue Buffer
 *
 * Copyright (c) 2018 Alexei A. Smekalkine <ikle at ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PACKET_BUF_H
#define PACKET_BUF_H  1

#include "bipbuf.h"

int packet_buf_is_empty (struct bipbuf *o);
int packet_buf_is_full  (struct bipbuf *o);

/*
 * 1. Reserve a packet region for writing: get a pointer to the buffer
 *    alongside with a room available for writing.
 * 2. Write some data into packet region.
 * 3. Commit the written data. The size should be less or equal to the
 *    reserved region size.
 */
void *packet_buf_reserve (struct bipbuf *o, size_t *size);
void  packet_buf_commit  (struct bipbuf *o, size_t  size);

/*
 * 1. Request a packet region for reading: get a pointer to the buffer
 *    alongside with a packet size.
 * 2. Read some data from requested packet region.
 * 3. Release packet data.
 */
void *packet_buf_request (struct bipbuf *o, size_t *size);
void  packet_buf_release (struct bipbuf *o);

#endif  /* PACKET_BUF_H */
