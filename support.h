/*
 * itop: Interrupt usage monitor
 * Copyright (C) 2002-2007 Kostas Evangelinos <kos@bastard.net>
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
 * $Id: support.h,v 1.2 2008-07-27 01:27:23 kos Exp $
 *
 */

#ifndef _SUPPORT_H
#define _SUPPORT_H

#include <time.h>

#ifndef HAVE_GETCWD
char *getcwd(char *buf, size_t size);
#endif

#ifndef HAVE_GETHOSTNAME
int gethostname(char *name, size_t len);
#endif

#ifndef HAVE_STRDUP
char *strdup(const char *s);
#endif

#ifndef HAVE_STRERROR
char *strerror(int errnum);
#endif

#ifndef HAVE_GMTIME_R
struct tm *gmtime_r(const time_t *timep, struct tm *tp);
#endif

#ifndef HAVE_LOCALTIME_R
struct tm *localtime_r(const time_t *timep, struct tm *tp);
#endif

#ifndef HAVE_VSYSLOG
void vsyslog(int, const char *, ...);
#endif

#ifndef HAVE_PCAP_DATALINK_VAL_TO_NAME
const char *pcap_datalink_val_to_name(int dlt);
#endif

#ifndef HAVE_PCAP_SET_DATALINK
int pcap_set_datalink(void *, int);
#endif

/* 
 * In case we have an older version of libpcap, this hack gives
 * us a chance of working and makes the code simpler
 */
#ifndef DLT_IEEE802_11_RADIO
#define DLT_IEEE802_11_RADIO 127
#endif

#ifndef DLT_PRISM_HEADER
#define DLT_PRISM_HEADER 119
#endif

#ifndef DLT_IEEE802_11
#define DLT_IEEE802_11 105
#endif

#ifndef DLT_IEEE802_11_RADIO_AVS
#define DLT_IEEE802_11_RADIO_AVS 163
#endif 

#ifndef DLT_EN10MB
#define DLT_EN10MB 1
#endif

#endif /* _SUPPORT_H */
