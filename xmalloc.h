/*
 * itop: Interrupt usage monitor
 * Copyright (C) 2002-2007 Kostas Evangelinos <kos@bastard.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

/*
 * $Id: xmalloc.h,v 1.1 2008-07-27 01:27:23 kos Exp $
 *
 */

#ifndef _XMALLOC_H
#define _XMALLOC_H

#include <stdlib.h>

typedef void (*xmalloc_fatal_t)(char *errstr, int code, int size);

void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
void xfree(void *ptr);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *s);

#endif /* _XMALLOC_H */
