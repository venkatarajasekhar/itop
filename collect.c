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
#include <sys/types.h>
#include <unistd.h>

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
collect_schema(int_collect_t* it) {
  FILE *fp;
  char line[DEF_LINELEN];
  int ntokens;
  int i, j;
  char *token;
  struct timeval now;
  int iline;

  if(!(fp = fopen(PROC_FILE, "r")))
    return 1;

  it->ncpus = 0;
  it->nlines = 0;
  
  /* figure out number of cpu's */
  fgets(line, DEF_LINELEN, fp);
  token = strtok(line, " :\n");
  if(!token)
    return 1;

  do {
    token = strtok(NULL, " ,\n");
    it->ncpus++;
  } while(token);

  /* figure out number of interrupt lines */
  while(fgets(line, DEF_LINELEN, fp)) {
    token = strtok(line, " :\n");
    if(!token)
      continue;
    if(atoi(token)==0 && strcmp(token, "0"))
      continue;
    it->nlines++;
  }

  it->cpu_ident = (char **)xmalloc(it->ncpus * sizeof(char*));
  it->max_tokens = 256+2*it->ncpus;
  it->tokens = (char **)xmalloc(it->max_tokens);
  it->line = (int_line_t *)xcalloc(it->nlines, sizeof(int_line_t));

  fseek(fp, 0, SEEK_SET);
  fgets(line, DEF_LINELEN, fp);
  tokenize(line, it->tokens, &ntokens, it->max_tokens);
  for(i=0; i<ntokens; i++)
    it->cpu_ident[i] = strdup(it->tokens[i]);

  gettimeofday(&now, NULL);
  i = 0;
  while(fgets(line, DEF_LINELEN, fp)) {
    int_line_t* interrupt = &it->line[i];
    tokenize(line, it->tokens, &ntokens, it->max_tokens);
    iline = atoi(it->tokens[0]);
    if(iline==0 && strcmp(it->tokens[0], "0")) 
      continue;
    if(ntokens > 0)
      strncpy(interrupt->name, it->tokens[0], DEF_LINELEN);
    else
      strcpy(interrupt->name, "unnamed");
    if(ntokens > it->ncpus+1)
      strncpy(interrupt->type, it->tokens[it->ncpus+1], DEF_LINELEN);
    else
      strcpy(interrupt->type, "unknown");
    interrupt->line = iline; 
    interrupt->count = 0; 
    for(j=0; j<it->ncpus; j++) 
      interrupt->count += atoll(it->tokens[j+1]);
    interrupt->rate = 0; 
    interrupt->ts_last_sample = now;
    i++;
  } 
  fclose(fp);
  
  return 0;
}

int_collect_t *
collect_new(void) {
  int_collect_t *ic;

  if(!(ic = xcalloc(1, sizeof(int_collect_t))))
    return (int_collect_t *)NULL;
  ic->rate = 0;
  ic->start = time(NULL);
  collect_schema(ic);
  collect_update(ic);
  return ic;
}

int
collect_update(int_collect_t *ic) {
  FILE *fp;
  char line[DEF_LINELEN];
  int ntokens;
  int n, i, iline;
  struct timeval now;
  uint64_t prevcount;
  
  if(!ic->ncpus)
    return 2;

  if(!(fp = fopen(PROC_FILE, "r")))
    return 1;

  gettimeofday(&now, NULL);
  ic->errors = 0;
  ic->rate = 0;

  /* discard first line: processors - assume this hasn't changed */
  fgets(line, DEF_LINELEN, fp);
  
  /* subsequent lines: interrupt counts */
  while(fgets(line, DEF_LINELEN, fp)) {
    tokenize(line, ic->tokens, &ntokens, ic->max_tokens);
    
    if(!strcmp(ic->tokens[0], "ERR") || !strcmp(ic->tokens[0], "MIS") || !strcmp(ic->tokens[0], "SPU")) {
      ic->errors += atoll(ic->tokens[1]);
      continue;
    }

    if(!atoi(ic->tokens[0]) && strcmp(ic->tokens[0], "0"))
      continue;

    iline = atoi(ic->tokens[0]);

    for(i=0; i<ic->nlines; i++) {
      int_line_t* interrupt = &ic->line[i];
      if(interrupt->line == -1)
	break;
      if(interrupt->line != iline)
	continue;

      prevcount = interrupt->count;
      interrupt->count = 0;
      for(n=0; n<ic->ncpus; n++)
	interrupt->count += atoll(ic->tokens[n+1]);
      
      interrupt->rate = (double)(interrupt->count - prevcount) / timeval_diff(now, interrupt->ts_last_sample);
      interrupt->ts_last_sample = now;
      interrupt->type[0] = '\0';
      interrupt->name[0] = '\0';
      if(ntokens > ic->ncpus+1)
	strncpy(interrupt->type, ic->tokens[ic->ncpus+1], DEF_LINELEN);
      else
	strcpy(interrupt->type, "unknown");

      if(ntokens > ic->ncpus+2)
	strncpy(interrupt->name, ic->tokens[ic->ncpus+2], DEF_LINELEN);
      else
	strcpy(interrupt->name, "unknown");
      
      ic->rate += interrupt->rate;
      for(n=ic->ncpus+3; n<ntokens; n++) {
	strncat(interrupt->name, "/", DEF_LINELEN);
	strncat(interrupt->name, ic->tokens[n], DEF_LINELEN);
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
