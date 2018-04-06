/*
 * Bipartite Buffer
 *
 * Copyright (c) 2014, 2015 Alexei A. Smekalkine <ikle at ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
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
