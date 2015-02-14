/*
 * Copyright (c) 2014, 2015 Alexei A. Smekalkine <ikle at ikle.ru>.
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

#include <assert.h>
#include <stdlib.h>
#include "bipbuf.h"

struct bipbuf {
	char *head, *tail, *a_head, *a_tail, *b_tail;
	int is_a_active;
};

struct bipbuf *bipbuf_alloc (size_t size)
{
	struct bipbuf *b;

	if ((b = malloc (sizeof (*b))) == NULL)
		goto no_object;

	if ((b->head = malloc (size)) == NULL)
		goto no_buffer;

	b->tail = b->head + size;
	b->a_head = b->a_tail = b->b_tail = b->head;
	b->is_a_active = 1;
	return b;

no_buffer:
	free (b);
no_object:
	return NULL;
}

void bipbuf_free (struct bipbuf *b)
{
	if (b == NULL)
		return;

	free (b->head);
	free (b);
}

static size_t bipbuf_get_a_avail (struct bipbuf *b)
{
	return b->tail - b->a_tail;
}

static size_t bipbuf_get_b_avail (struct bipbuf *b)
{
	return b->a_head - b->b_tail;
}

int bipbuf_is_empty (struct bipbuf *b)
{
	return b->a_head == b->a_tail;
}

int bipbuf_is_full (struct bipbuf *b)
{
	return !b->is_a_active ?
		bipbuf_get_b_avail (b) == 0 :
		bipbuf_get_a_avail (b) == 0;
}

void *bipbuf_reserve (struct bipbuf *b, size_t *size)
{
	if (!b->is_a_active) {
		*size = bipbuf_get_b_avail (b);
		return b->b_tail;
	}

	*size = bipbuf_get_a_avail (b);
	return b->a_tail;
}

void bipbuf_commit (struct bipbuf *b, size_t size)
{
	if (!b->is_a_active) {
		b->b_tail += size;
		assert (b->b_tail <= b->a_head);
		return;
	}

	b->a_tail += size;
	assert (b->a_tail <= b->tail);

	if (bipbuf_get_b_avail (b) > bipbuf_get_a_avail (b))
		b->is_a_active = 0;
}

void *bipbuf_request (struct bipbuf *b, size_t *size)
{
	*size = b->a_tail - b->a_head;
	return b->a_head;
}

void bipbuf_release (struct bipbuf *b, size_t size)
{
	b->a_head += size;
	assert (b->a_head <= b->a_tail);

	if (b->a_head < b->a_tail)  /* is A region not empty? */
		return;

	/* move B region to A */
	b->a_head = b->head;
	b->a_tail = b->b_tail;
	b->b_tail = b->head;
	b->is_a_active = 1;
}
