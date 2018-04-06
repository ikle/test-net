/*
 * Packet Queue Buffer
 *
 * Copyright (c) 2018 Alexei A. Smekalkine <ikle at ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>

#include "packet-buf.h"

int packet_buf_is_empty (struct bipbuf *o)
{
	return bipbuf_is_empty (o);
}

int packet_buf_is_full (struct bipbuf *o)
{
	size_t avail;

	(void) bipbuf_reserve (o, &avail);

	return avail <= 2;
}

void *packet_buf_reserve (struct bipbuf *o, size_t *size)
{
	void *p;
	size_t avail;

	p = bipbuf_reserve (o, &avail);
	if (avail <= 2) {
		*size = 0;
		return NULL;
	}

	avail -= 2;
	if (avail > 0xffff)
		avail = 0xffff;

	*size = avail;
	return p + 2;
}

void packet_buf_commit (struct bipbuf *o, size_t size)
{
	unsigned char *p;
	size_t avail;

	assert (size <= 0xffff);

	p = bipbuf_reserve (o, &avail);
	assert (avail >= (2 + size));

	p[0] = size;
	p[1] = size >> 8;

	bipbuf_commit (o, size + 2);
}

void *packet_buf_request (struct bipbuf *o, size_t *size)
{
	unsigned char *p;
	size_t avail;

	p = bipbuf_request (o, &avail);
	if (avail < 2) {
		*size = 0;
		return NULL;
	}

	*size = p[0] | (p[1] << 8);
	return p + 2;
}

void packet_buf_release (struct bipbuf *o)
{
	unsigned char *p;
	size_t avail, size;

	p = bipbuf_request (o, &avail);
	assert (avail >= 2);

	size = p[0] | (p[1] << 8);
	assert (avail >= (size + 2));

	bipbuf_release (o, size + 2);
}
