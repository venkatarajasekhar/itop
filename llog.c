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
 * $Id: llog.c,v 1.2 2008-07-27 01:27:23 kos Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <syslog.h>

#include "llog.h"

LOG_LEVEL_T level;
LOG_METHOD_T method;
FILE *log_fp;
char debugbuf[8192];
int priority;      /* for syslog */

LOG_LEVEL_T 
llog_getlevel(void) {
  return level;
}

int
llog_init(LOG_METHOD_T newmethod, ...) {
  int retvalue;
  va_list vl;
  int facility;
  int options;
  int lpriority;
  char *ident;
  char *file;
  char *perm;

  va_start(vl, newmethod);

  method = newmethod;

  switch(method) {
  case LLOG_STDERR:
    log_fp = stderr;
    retvalue = 0;
    break;

  case LLOG_SYSLOG:
    ident = va_arg(vl, caddr_t);
    options = va_arg(vl, int);
    facility = va_arg(vl, int);
    lpriority = va_arg(vl, int);
    openlog(ident, options, facility);
    priority = lpriority;
    retvalue = 0;
    break;

  case LLOG_FILE:   
    file = va_arg(vl, caddr_t);
    perm = va_arg(vl, caddr_t);
    if(!(log_fp = fopen(file, perm)))
      retvalue = 1;
    else
      retvalue = 0;
    break;
  default:
    retvalue = 0;
    break;
  }
  
  level = 0;
  return retvalue;
}

void
llog(LOG_LEVEL_T userlevel, char *form, ...) {
  va_list vl;
  va_start(vl, form);

  if(userlevel > level)
    return;

  switch(method) {
  case LLOG_STDERR:
  case LLOG_FILE:
    vfprintf(log_fp, form, vl);
    //fprintf(log_fp, form);
    //puts(form);
    fflush(log_fp);
    break;
  case LLOG_SYSLOG:
#ifdef HAVE_SYSLOG_H
    vsyslog(priority, form, vl);
#endif
    break;

  default:
    break;
  }

  va_end(vl);
}

void
llog_direct(LOG_LEVEL_T userlevel, char *form, ...) {

  if(userlevel > level)
    return;

  switch(method) {
  case LLOG_STDERR:
  case LLOG_FILE:
    fwrite(form, strlen(form), 1, log_fp);
    break;
  default:
    break;
  }
}

void
llog_hexdump(int userlevel, char *buffer, int len) {
  int i, j;
  char line[1024];
  char digit[64];

  if(userlevel > level)
    return;

  for(j=0; j<(len/16)+1; j++) {
#ifdef LLOG_HEX_HEADER
    if(!(j%8))
      llog_direct(level, "          00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F\n");
#endif

    sprintf(line, "%08x  ", j*16);
    for(i=0; i<16; i++) {
      if((j*16 + i) >= len)
	sprintf(digit, "   ");
      else 
	sprintf(digit, "%02x ", (unsigned char)buffer[(j*16) + i]);
      strcat(line, digit);
    }
    strcat(line, "    ");
    for(i=0; i<16; i++) {
      if((j*16 + i) >= len) {
	digit[0] = ' ';
	digit[1] = '\0';
      } else {
	digit[0] = buffer[(j*16) + i];
	if(digit[0] <32 || digit[0] >=126)
	  digit[0] = '.';
	digit[1] = '\0';
      }
      strcat(line, digit);
    }
    llog_direct(level, line);
    llog_direct(level, "\n");
  }  
}

void
llog_level(LOG_LEVEL_T new) {
  level = new;
}

void
llog_close(void) {
  
  switch(method) {
  case LLOG_STDERR:
  case LLOG_FILE:
    fclose(log_fp);
    break;
  case LLOG_SYSLOG:
#ifdef HAVE_SYSLOG_H
    closelog();
#endif
    break;
  }
}

#ifdef LOG_DEBUG_MODE
int
main(void) {
  log_init(LLOG_STDERR);
  log(1, "level1");
  log(2, "level2");
  log_level(2);
  log(3, "level3 %d %d %d\n", 1, 2, 3);
  log(1, "level1");
  log_close();

  log_init(LLOG_SYSLOG, "test1", LOG_PID, LOG_DAEMON);
  log_level(3);
  log(4, "log4 %s %m\n", "help");
  log(5, "log5\n");
  log_close();
}
#endif
