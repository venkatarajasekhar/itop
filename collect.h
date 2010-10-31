/*
 * itop: Interrupt usage monitor
 * Copyright (C) 2008 Kostas Evangelinos <kos@bastard.net>
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
 * $Id: collect.h,v 1.8 2008-07-27 02:38:11 kos Exp $
 *
 */

#include <time.h>
#include <stdint.h>

#include "util.h"
#define PROC_FILE "/proc/interrupts"

typedef struct {
  int line;
  char type[DEF_LINELEN];
  char name[DEF_LINELEN];
  uint64_t count;
  double rate;
  struct timeval ts_last_sample;
} int_line_t;

typedef struct {
  unsigned int ncpus;
  unsigned int nlines;
  uint64_t errors;
  uint64_t misses;
  uint64_t nmis;
  uint64_t locs;
  time_t start;
  char** cpu_ident;
  int_line_t* line;
  double rate;
  int max_tokens;
  char **tokens;
} int_collect_t;

int_collect_t *collect_new();
int collect_update(int_collect_t *);
int collect_destroy(int_collect_t *);
