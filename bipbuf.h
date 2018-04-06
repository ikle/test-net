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
