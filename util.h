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
 * $Id: util.h,v 1.5 2008-07-27 02:38:11 kos Exp $
 *
 */

#ifndef _UTIL_H
#define _UTIL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ENDIAN_H
#include <endian.h>
#endif

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#define DEF_LINELEN 1024
#define STRINGS_LEN 8
#define STRINGS_BUF (80*22)    /* at least cover 80x22 screen size */
#define ISSTRING(x) (x>32 && x!=127)

#ifndef __BYTE_ORDER
#define __LITTLE_ENDIAN 1
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif

#if __BYTE_ORDER==__LITTLE_ENDIAN 
#define EXTRACT_LE_8BITS(p) (*(p))
#define EXTRACT_LE_16BITS(p) (*(p))
#define EXTRACT_LE_32BITS(p) (*(p))
#define EXTRACT_LE_64BITS(p) (*(p))
#else
#define EXTRACT_LE_8BITS(p) (*(p))
#define EXTRACT_LE_16BITS(p) \
        ((u_int16_t)((u_int16_t)*((const u_int8_t *)(p) + 1) << 8 | \
                     (u_int16_t)*((const u_int8_t *)(p) + 0)))
#define EXTRACT_LE_32BITS(p) \
        ((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 3) << 24 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 2) << 16 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 1) << 8 | \
                     (u_int32_t)*((const u_int8_t *)(p) + 0)))
#define EXTRACT_LE_64BITS(p) \
        ((u_int64_t)((u_int64_t)*((const u_int8_t *)(p) + 7) << 56 | \
                     (u_int64_t)*((const u_int8_t *)(p) + 6) << 48 | \
                     (u_int64_t)*((const u_int8_t *)(p) + 5) << 40 | \
                     (u_int64_t)*((const u_int8_t *)(p) + 4) << 32 | \
                     (u_int64_t)*((const u_int8_t *)(p) + 3) << 24 | \
                     (u_int64_t)*((const u_int8_t *)(p) + 2) << 16 | \
                     (u_int64_t)*((const u_int8_t *)(p) + 1) << 8 | \
                     (u_int64_t)*((const u_int8_t *)(p) + 0)))
#endif /* __BYTE_ORDER */

#undef  MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#undef  MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))

typedef struct strings_t {
  char strings[STRINGS_BUF];
  int start;
} strings_t;

void tokenize(char *usrc, char **tokens, int *ntokens, int max_tokens);
char *geek_print(uint64_t);
char *mac2ascii(unsigned char *);
char *hex2ascii(unsigned char *, int);
char *string_printable(char *);
char *uptime(time_t);
int strings_clear(strings_t *s);
int strings_add(strings_t *s, char *p, int l);
int endian_islittle(void);
void endian_swab(unsigned char *, int);
char *human_time_delta(struct timeval *);

#endif /* _UTIL_H */
