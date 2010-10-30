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
 * $Id: llog.h,v 1.2 2008-07-27 01:27:23 kos Exp $
 *
 */

#ifndef _LOG_H
#define _LOG_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#define LOG_LEVEL_T unsigned int
#define LOG_METHOD_T unsigned short int

#define LLOG_STDERR 0
#define LLOG_SYSLOG 1
#define LLOG_FILE   2

int llog_init(LOG_METHOD_T, ...);
LOG_LEVEL_T llog_getlevel(void);
void llog(LOG_LEVEL_T, char *, ...);
void llog_level(LOG_LEVEL_T);
void llog_close(void);
void llog_hexdump(int level, char *buffer, int len);
#ifdef __cplusplus
	   }
#endif

#endif
