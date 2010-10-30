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
 * $Id: xmalloc.c,v 1.1 2008-07-27 01:27:23 kos Exp $
 *
 */

#include <unistd.h>
#include <string.h>

#include "xmalloc.h"
#include "llog.h"

void
xmalloc_fatal_default(char *errstr, int code, int size) {
  llog(0, "%s: failed to allocate %d bytes\n", errstr, size);
  exit(code);
}

xmalloc_fatal_t xmalloc_fatal = xmalloc_fatal_default;

void *
xmalloc(size_t size) {
  void *p;

  if(!(p = malloc(size)))
    xmalloc_fatal("xmalloc", 1, size);
  llog(9, "xmalloc(%d): %p\n", size, p);
  return p;
}

void *
xcalloc(size_t nmemb, size_t size) {
  void *p;
  
  if(!(p = calloc(nmemb, size)))
    xmalloc_fatal("xcalloc", 1, size*nmemb);
  llog(9, "xcalloc(%d): %p\n", nmemb*size, p);
  return p;
}

void *
xrealloc(void *ptr, size_t size) {
  void *p;

  if(!(p = realloc(ptr, size)))
    xmalloc_fatal("xrealloc", 1, size);
  llog(9, "xrealloc(%p, %d): %p\n", ptr, size, p);
  return p;
}

char *
xstrdup(const char *s) {
  char *p;

  if(!(p = strdup(s)))
    xmalloc_fatal("xstrdup", 1, strlen(s));
  llog(9, "xstrdup(): %p\n", p);
  return p;
}

void
xfree(void *ptr) {
  free(ptr);
  llog(9, "xfree(%p)\n", ptr);
}
