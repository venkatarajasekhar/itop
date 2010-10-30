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
 * $Id: cursesui.h,v 1.4 2008-07-27 01:27:23 kos Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "util.h"

#ifndef _CURSESUI_H
#define _CURSESUI_H

#define UI_DIR_UP 0
#define UI_DIR_DOWN 1

typedef enum {
  UI_SORT_NAME, UI_SORT_RATE, UI_SORT_TOTAL, UI_SORT_LINE, UI_SORT_TYPE
} ui_sort_t;

typedef enum {
  UI_MODE_NORMAL, UI_MODE_HELP, UI_MODE_SORT
} ui_mode_t;

typedef struct uifield_t {
  char name[DEF_LINELEN];
  int size;
  int cursize;
  int selected;
} uifield_t;

int ui_init(void);
int ui_refresh();
int ui_redraw();
int ui_mode(ui_mode_t);
int ui_finish(void);
int ui_clear(void);
int ui_process_events();
int ui_info(char *, ...);

#endif /* _CURSESUI_H */
