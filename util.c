/*
 * itop: Interrupt usage monitor
 * Copyright (C) 2008 Kostas Evangelinos <kos@bastard.net>
 *
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
 * $Id: util.c,v 1.8 2008-07-27 02:38:11 kos Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "util.h"

void
tokenize(char *usrc, char **tokens, int *ntokens) {
  int i;
  static char src[DEF_LINELEN];
  
  strncpy(src, usrc, DEF_LINELEN);
  for(i=1,tokens[0] = strtok(src, " :\n"); (tokens[i]=strtok(NULL, " ,\n")); i++)
    ;
  *ntokens = i;
}

char *
geek_print(uint64_t n) {
  static char buf[DEF_LINELEN];
  
  if(n < 1024)
    snprintf(buf, DEF_LINELEN, "%llu", (unsigned long long)n);
  else if(n<1048576)
    snprintf(buf, DEF_LINELEN, "%.2fK", (float)n/(float)1024);
  else if(n<1073741824)
    snprintf(buf, DEF_LINELEN, "%.2fM", (float)n/(float)1048576);
  else
    snprintf(buf, DEF_LINELEN, "%.2fG", (float)n/(float)1073741824);
  return buf;
}

char *
uptime(time_t start) {
  static char buf[DEF_LINELEN];
  time_t uptime;
  struct tm *tm;

  uptime = time(NULL) - start;
  tm = gmtime(&uptime);
  if(tm->tm_yday)
    snprintf(buf, DEF_LINELEN, "%d days, %02d:%02d:%02d", 
	     tm->tm_yday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  else
    snprintf(buf, DEF_LINELEN, "%02d:%02d:%02d", 
	     tm->tm_hour, tm->tm_min, tm->tm_sec);
  return buf;
}

char *
mac2ascii(unsigned char *p) {
  static char out[DEF_LINELEN];
  
  snprintf(out, DEF_LINELEN, "%02x:%02x:%02x:%02x:%02x:%02x", 
	   (unsigned char)p[0], 
	   (unsigned char)p[1], 
	   (unsigned char)p[2], 
	   (unsigned char)p[3], 
	   (unsigned char)p[4], 
	   (unsigned char)p[5]);
  return out;
}

char *
hex2ascii(unsigned char *p, int l) {
  int i;
  static char out[DEF_LINELEN];
  char temp[DEF_LINELEN];

  out[0] = '\0';
  for(i=0; i<l; i++) {
    snprintf(temp, DEF_LINELEN, "%02x", p[i]);
    strncat(out, temp, DEF_LINELEN);
  }
  return out;
}

char *
string_printable(char *in) {
  static char out[DEF_LINELEN];
  char octal[DEF_LINELEN];
  char *ret;

  memset(out, '\0', DEF_LINELEN);
  ret = out;

  while(*in) {
    if(*in >= 32 && *in != 127) {
      *ret++ = *in;
    } else if(*in == '\\') {
      *ret++ = '\\';
      *ret++ = '\\';
    } else if(*in == '\a') {
      *ret++ = '\\';
      *ret++ = 'a';
    } else if(*in == '\b') {
      *ret++ = '\\';
      *ret++ = 'b';
    } else if(*in == '\n') {
      *ret++ = '\\';
      *ret++ = 'n';
    } else if(*in == '\r') {
      *ret++ = '\\';
      *ret++ = 'r';
    } else {
      snprintf(octal, DEF_LINELEN, "\\%03o", (unsigned char)*in);
      strncat(ret, octal, strlen(octal));
      ret += strlen(octal);
    }
    in++;
  }
  return out;
}

int
strings_clear(strings_t *s) {
  memset(s->strings, 0x20, sizeof(s->strings));
  s->start = 0;
  return 0;
}

int
strings_add(strings_t *s, char *p, int l) {
  char temp[DEF_LINELEN];
  int i;
  int j, k;

  j = 0;
  for(i=0; i<l; i++) {

    if(j == DEF_LINELEN) {
      for(k=0; k<j; k++) {
	s->strings[s->start] = temp[k];
	s->start = (s->start + 1) % sizeof(s->strings); 
      }
      j=0;
    }

    if(ISSTRING(p[i]))
      temp[j++] = p[i];
    else {      
      if(j >= STRINGS_LEN) {
	for(k=0; k<j; k++) {
	  s->strings[s->start] = temp[k];
	  s->start = (s->start + 1) % sizeof(s->strings);
	}
	s->strings[s->start] = ' ';
	s->start = (s->start + 1) % sizeof(s->strings);
      }
      j = 0;
    }
  }
  return 0;
}

int
endian_islittle(void) {
  short word = 0x4321;

  if((*(char *)& word) != 0x21 )
    return 0;
  else 
    return 1;
}

void
endian_swab(unsigned char *p, int l) {
  int i=0;
  int j=l-1;
  unsigned char temp;

  while (i<j) {
    temp = p[j];
    p[j] = p[i];
    p[i] = temp;
    i++, j--;
  }
}

char *
human_time_delta(struct timeval *tv) {
  static char out[DEF_LINELEN];
  char temp[DEF_LINELEN];
  time_t delta;
  int i;

  if(!tv->tv_sec)
    return "N/A";

  out[0] = '\0';
  delta = time(NULL) - tv->tv_sec;

  if(!delta)
    return "now";

  if((i = (delta / (60*60*24)))) {
    sprintf(temp, "%u days ", i);
    strcat(out, temp);
    delta -= i*60*60*24;
  } else if((i = (delta / (60*60)))) {
    sprintf(temp, "%u hours ", i);
    strcat(out, temp);
    delta -= i*60*60;
  } else if((i = (delta / 60))) {
    sprintf(temp, "%d minutes ", i);
    strcat(out, temp);
    delta -= i*60;
  } else if(delta) {
    sprintf(temp, "%d seconds ", (int)delta);
    strcat(out, temp);
  }
	strcat(out, "ago");
  return out;
}
