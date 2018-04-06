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
	struct bipbuf *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		goto no_object;

	if ((o->head = malloc (size)) == NULL)
		goto no_buffer;

	o->tail = o->head + size;
	o->a_head = o->a_tail = o->b_tail = o->head;
	o->is_a_active = 1;
	return o;

no_buffer:
	free (o);
no_object:
	return NULL;
}

void bipbuf_free (struct bipbuf *o)
{
	if (o == NULL)
		return;

	free (o->head);
	free (o);
}

static size_t bipbuf_get_a_avail (struct bipbuf *o)
{
	return o->tail - o->a_tail;
}

static size_t bipbuf_get_b_avail (struct bipbuf *o)
{
	return o->a_head - o->b_tail;
}

int bipbuf_is_empty (struct bipbuf *o)
{
	return o->a_head == o->a_tail;
}

int bipbuf_is_full (struct bipbuf *o)
{
	return !o->is_a_active ?
		bipbuf_get_b_avail (o) == 0 :
		bipbuf_get_a_avail (o) == 0;
}

void *bipbuf_reserve (struct bipbuf *o, size_t *size)
{
	if (!o->is_a_active) {
		*size = bipbuf_get_b_avail (o);
		return o->b_tail;
	}

	*size = bipbuf_get_a_avail (o);
	return o->a_tail;
}

void bipbuf_commit (struct bipbuf *o, size_t size)
{
	if (!o->is_a_active) {
		o->b_tail += size;
		assert (o->b_tail <= o->a_head);
		return;
	}

	o->a_tail += size;
	assert (o->a_tail <= o->tail);

	if (bipbuf_get_b_avail (o) > bipbuf_get_a_avail (o))
		o->is_a_active = 0;
}

void *bipbuf_request (struct bipbuf *o, size_t *size)
{
	*size = o->a_tail - o->a_head;
	return o->a_head;
}

void bipbuf_release (struct bipbuf *o, size_t size)
{
	o->a_head += size;
	assert (o->a_head <= o->a_tail);

	if (o->a_head < o->a_tail)  /* is A region not empty? */
		return;

	/* move B region to A */
	o->a_head = o->head;
	o->a_tail = o->b_tail;
	o->b_tail = o->head;
	o->is_a_active = 1;
}
