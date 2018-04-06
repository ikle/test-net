/*
 * Bipartite Buffer
 *
 * Copyright (c) 2014, 2015 Alexei A. Smekalkine <ikle at ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _BIPBUF_H
#define _BIPBUF_H  1

#include <stddef.h>

struct bipbuf *bipbuf_alloc (size_t size);
void bipbuf_free (struct bipbuf *b);

int bipbuf_is_empty (struct bipbuf *b);
int bipbuf_is_full  (struct bipbuf *b);

/*
 * 1. Reserve a region for writing: get a pointer to the buffer alongside
 *    with a room available for writing.
 * 2. Write some data into region.
 * 3. Commit the written data. The size should be less or equal to the
 *    reserved region size.
 */
void *bipbuf_reserve (struct bipbuf *b, size_t *size);
void  bipbuf_commit  (struct bipbuf *b, size_t  size);

/*
 * 1. Request a region for reading: get a pointer to the buffer alongside
 *    with a room available for reading.
 * 2. Read some data from requested region.
 * 3. Release readed data. The size should be less or equal to the requested
 *    region size.
 */
void *bipbuf_request (struct bipbuf *b, size_t *size);
void  bipbuf_release (struct bipbuf *b, size_t  size);

#endif  /* _BIPBUF_H */
