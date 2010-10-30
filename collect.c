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
 * $Id: collect.c,v 1.9 2008-07-27 02:38:11 kos Exp $
 *
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "util.h"
#include "collect.h"
#include "xmalloc.h"
#include "util.h"

double
timeval_diff(struct timeval t1, struct timeval t2) {
  double diff;
  
  diff = ((double)t2.tv_usec - (double)t1.tv_usec)/1000000 + 
    ((double)t2.tv_sec - (double)t1.tv_sec);
  return fabs(diff);
}

static int
collect_initial(int_collect_t *ic) {
  FILE *fp;
  char line[DEF_LINELEN];
  char *tokens[16];
  int ntokens;
  int iline;
  int i;
  struct timeval now;

  if(!(fp = fopen(PROC_FILE, "r")))
    return 1;

  gettimeofday(&now, NULL);

  /* first line: processors */
  fgets(line, DEF_LINELEN, fp);
  if(ic->nprocs == 0) {
    tokenize(line, tokens, &ntokens);
    ic->nprocs = ntokens;
  }

  /* subsequent lines define the interrupt lines */
  i = 0;
  while(fgets(line, DEF_LINELEN, fp)) {
    tokenize(line, tokens, &ntokens);
    if(!strcmp(tokens[0], "0"))
      iline = 0; 
    else if(!(iline = atoi(tokens[0])))
      break;

    ic->line[i].tvp = now;
    ic->nlines++;
    ic->line[i++].line = iline;
  }
  fclose(fp);

  ic->start = time(NULL);  
  return collect_update(ic);
}

int_collect_t *
collect_new(void) {
  int_collect_t *ic;
  int i;

  if(!(ic = xcalloc(1, sizeof(int_collect_t))))
    return (int_collect_t *)NULL;
  for(i=0; i<COLLECT_MAX; i++)
    ic->line[i].line = -1;
  ic->rate = 0;
  collect_initial(ic);
  return ic;
}

int
collect_update(int_collect_t *ic) {
  FILE *fp;
  char line[DEF_LINELEN];
  char *tokens[16];
  int ntokens;
  int n, i, iline;
  struct timeval now;
  uint64_t prevcount;
  
  if(!ic->nprocs)
    return 2;

  if(!(fp = fopen(PROC_FILE, "r")))
    return 1;

  gettimeofday(&now, NULL);
  ic->errors = 0;
  ic->rate = 0;

  /* first line: processors */
  fgets(line, DEF_LINELEN, fp);
  
  /* subsequent lines: interrupt counts */
  while(fgets(line, DEF_LINELEN, fp)) {
    tokenize(line, tokens, &ntokens);
    
    if(!strcmp(tokens[0], "ERR")) {
      ic->errors += atoll(tokens[1]);
      continue;
    }

    if(!strcmp(tokens[0], "MIS")) {
      ic->errors += atoll(tokens[1]);
      continue;
    }

    if(!atoi(tokens[0]) && strcmp(tokens[0], "0"))
      continue;

    iline = atoi(tokens[0]);

    for(i=0; i<COLLECT_MAX && ic->line[i].line != -1; i++) {
      if(ic->line[i].line == iline) {

	prevcount = ic->line[i].count;
	ic->line[i].count = 0;

	for(n=0; n<ic->nprocs; n++)
	  ic->line[i].count += atoll(tokens[n+1]);	  
	
	ic->line[i].rate = (double)(ic->line[i].count - prevcount) / timeval_diff(now, ic->line[i].tvp);
	ic->line[i].tvp = now;
	strncpy(ic->line[i].type, tokens[ic->nprocs+1], DEF_LINELEN);
	strncpy(ic->line[i].name, tokens[ic->nprocs+2], DEF_LINELEN);
	ic->rate += ic->line[i].rate;
	for(n=ic->nprocs+3; n<ntokens; n++) {
	  strncat(ic->line[i].name, "/", DEF_LINELEN);
	  strncat(ic->line[i].name, tokens[n], DEF_LINELEN);
	}
      }
    }
  }
  fclose(fp);
  return 0;
}

int
collect_destroy(int_collect_t *ic) {
  xfree(ic);
  return 0;
}
