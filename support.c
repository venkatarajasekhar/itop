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
 * $Id: support.c,v 1.2 2008-07-27 01:27:23 kos Exp $
 *
 */

#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "support.h"


#ifndef HAVE_GETCWD
char *
getcwd(char *buf, size_t size) {
  if(size<2)
    return NULL;
  buf[0] = '/';
  buf[1] = '\0';
  return buf;
}
#endif

#ifndef HAVE_GETHOSTNAME
int
gethostname(char *name, size_t len) {
  strncpy(name, "node", len);
  return 0;
}
#endif

#ifndef HAVE_STRDUP
char *
strdup(const char *s) {
  char *r;
  if(!(r = (char *)malloc(strlen(s))))
    return NULL;
  return r;
}
#endif

#ifndef HAVE_STRERROR
char *
strerror(int errnum) {
  if(errnum>sys_nerr)
    return "No such error";
  return sys_errlist[errnum];
}
#endif

#ifndef HAVE_SNPRINTF
#warning "You dont have snprintf(). The resulting executable is not secure and should not be used"
#define snprintf sprintf
#endif

#ifndef HAVE_GMTIME_R
struct tm *
gmtime_r(const time_t *timep, struct tm *tp) {
  struct tm *t;

  if(!(t = gmtime(timep)))
    return (struct tm *)NULL;
  memcpy((void *)tp, (void *)t, sizeof(struct tm));
  return tp;
}
#endif

#ifndef HAVE_LOCALTIME_R
struct tm *
localtime_r(const time_t *timep, struct tm *tp) {
  struct tm *t;

  if(!(t = localtime(timep)))
    return (struct tm *)NULL;
  memcpy((void *)tp, (void *)t, sizeof(struct tm));
  return tp;
}
#endif

#ifndef HAVE_VSYSLOG
#include <stdarg.h>
#ifdef HAVE_SYSLOG
#include <syslog.h>
#endif
#include "common.h"

void
vsyslog(int pri, const char *fmt, ...) {
  va_list vl;
  uchar fmtstr[DEF_LINELEN];

  va_start(vl, fmt);
  vsnprintf(fmtstr, DEF_LINELEN, fmt, vl);
  va_end(vl);
  syslog(pri, fmtstr, strlen(fmtstr));  
}
#endif

#ifndef HAVE_PCAP_DATALINK_VAL_TO_NAME
const char *
pcap_datalink_val_to_name(int dlt) {
  
  switch(dlt) {
  case DLT_EN10MB:
    return "EN10MB";
    break;
  case DLT_IEEE802_11:
    return "IEEE802_11";
    break;
  case DLT_IEEE802_11_RADIO:
    return "IEEE802_11_RADIO";
    break;
  case DLT_IEEE802_11_RADIO_AVS:
    return "IEEE802_11_RADIO_AVS";
    break;
  case DLT_PRISM_HEADER:
    return "PRISM_HEADER";
    break;
  default:
    return "Unknown";
  }
}
#endif

#ifndef HAVE_PCAP_SET_DATALINK
int
pcap_set_datalink(void *p, int dlt) {
	return 0;
}
#endif
