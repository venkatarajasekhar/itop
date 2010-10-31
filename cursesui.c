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
 * $Id: cursesui.c,v 1.12 2008-07-29 00:46:11 kos Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <sys/param.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/utsname.h>

#include "cursesui.h"
#include "util.h"
#include "collect.h"

#define UI_STATUS_POS 0
#define UI_HEADER_POS 1
#define UI_MAIN_POS 2

static char rotseq[] = "/-\\|";
static int disp_mode;
static int sort_mode;
static char line_info[DEF_LINELEN];
static int disp_offset;
static time_t full_update;

extern int_collect_t *ic;
struct utsname un;
extern int running;

/* 
 * XXX: Order is important until this is done better.
 */

uifield_t uifield[] = {
  {"Interrupt name", 40, 0},
  {"Rate", 8, 0},
  {"Total", 10, 0},
  {"Line", 6, 0},
  {"Type", 12, 0},
  {"", 0, 0, 0},
};

int
field_get_value(int field_id, int_line_t *sample, char *res, int resl) {
  
  *res = '\0';
  switch(field_id) {
  case 0:
    snprintf(res, resl, "%-40s", sample->name);
    break;
  case 1:
    snprintf(res, resl, "%-.1f", sample->rate);
    break;
  case 2:
    snprintf(res, resl, "%-13s", geek_print(sample->count));
    break;
  case 3:
    snprintf(res, resl, "%-4d", sample->line);
    break;
  case 4:
    snprintf(res, resl, "%-12s", sample->type);
    break;
  }
  return 0;
}

static int
sortfunc(int_line_t *a, int_line_t *b) {
  switch(sort_mode) {
  case UI_SORT_NAME:
    return strcmp(a->name, b->name);
    break;
  case UI_SORT_RATE:
    return (a->rate < b->rate);
    break;
  case UI_SORT_TOTAL:
    return (a->count < b->count);
    break;
  case UI_SORT_LINE:
    return (a->line < b->line);
    break;
 case UI_SORT_TYPE:
    return strcmp(a->type, b->type);
    break;
}
  return 0;
}

static int
header_show() {
  static int spindex = 0;
  char line[DEF_LINELEN];
  
  move(UI_STATUS_POS, 0);
  snprintf(line, DEF_LINELEN, "%s %c %s %s, %u cpu%s, %llu errors, %.1f int/s",
	   un.nodename, rotseq[(spindex)%4], un.sysname, un.release,
	   ic->ncpus, ic->ncpus>1?"s":"", ic->errors, ic->rate);
  printw(line);
  move(UI_STATUS_POS+1, 0);

  spindex++;
  return 0;
}

static int
list_show() {
  int i;
  int field_id;
  int column;
  char header[DEF_LINELEN];
  char format[DEF_LINELEN];
  char value[DEF_LINELEN];
  
  qsort(ic->line, ic->nlines, sizeof(int_line_t), 
	(int(*)(const void *, const void *))sortfunc);
  
  // blank header and figure out the fields we can show
  header[0] = '\0';
  column=1;
  for(field_id=0; uifield[field_id].size; field_id++) {
    if(uifield[field_id].size + column > COLS) {
      uifield[field_id].cursize = 0;
      break;
    }
    column += uifield[field_id].size;
    uifield[field_id].cursize = uifield[field_id].size;
  }
  
  // print header
  move(UI_HEADER_POS, 0);
  attron(A_REVERSE);
  for(i=0; i<COLS; i++)
    printw(" ");
  move(UI_HEADER_POS, 0);
  for(field_id=0; uifield[field_id].cursize; field_id++) {
    snprintf(format, DEF_LINELEN, "%%-%ds", uifield[field_id].cursize);
    printw(format, uifield[field_id].name);
  }
  attroff(A_REVERSE);  

  // print content
  for(i=0; i < LINES; i++) {
    
    if(i >= ic->nlines)
      break;

    column=0;
    for(field_id=0; uifield[field_id].cursize; field_id++) {
      move(UI_MAIN_POS+i, column);
      field_get_value(field_id, &ic->line[i], value, DEF_LINELEN);
      printw(value);
      column += uifield[field_id].size;
    }
  }
  move(UI_STATUS_POS, COLS-1);
  return 0;
}

static int
help_show() {
  int i;

  move(UI_HEADER_POS, 0);
  attron(A_REVERSE);
  for(i=0; i<COLS; i++)
    printw(" ");
  move(UI_HEADER_POS, 0);
  printw("Help Page | %s %s", PACKAGE, VERSION);
  attroff(A_REVERSE);  
  
  move(UI_MAIN_POS, 0);
  //----------------------------------------------------------------------------
  printw("\
Space  Update screen\n\
o      Select sort order: \n\
           as (n)ame, (r)ate, (t)otal, (l)ine, ty(p)e \n\
^L     Redraw screen\n\
q      Quit\n\
h      Help\n\
\n");
  move(UI_STATUS_POS, COLS-1);
  return 0;
}

int
ui_init(void) {  
  
  uname(&un);
  disp_mode = UI_MODE_NORMAL;
  sort_mode = UI_SORT_RATE;
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  line_info[0] = '\0';
  disp_offset = 0;
  full_update = 0;
  return 0;
}

int
ui_info(char *fmt, ...) {
  va_list vl;

  va_start(vl, fmt);
  vsnprintf(line_info, DEF_LINELEN, fmt, vl);
  va_end(vl);
  return 0;
}

int
ui_mode(ui_mode_t umode) {
  disp_mode = umode;
  return 0;
}

int
ui_finish(void) {
  endwin();
  return 0;
}

int
ui_clear(void) {
  erase();
  return 0;
}

int
ui_sort(ui_sort_t mode) {
  sort_mode = mode;
  return 0;
}

int
ui_process_events() {
  int need_redraw = 0;
  
  switch(disp_mode) {

  case UI_MODE_NORMAL:
    switch(getch()) {
    case 014: /* ^L */
      clear();
      need_redraw = 1;
      break;
    case 'q': /* quit */
      running = 0;
      break;
    case 'h':
    case '?': /* help */
      disp_mode = UI_MODE_HELP;
      need_redraw = 1;
      break;
    case ' ': /* refresh */
      break;
    case 'o': /* sort order */
      disp_mode = UI_MODE_SORT;
      need_redraw = 1;
      break;
    }
    break;
  case UI_MODE_HELP:
    getch();
    disp_mode = UI_MODE_NORMAL;
    need_redraw = 1;
    break;
  case UI_MODE_SORT:
    switch(getch()) {
    case 'n':
      ui_sort(UI_SORT_NAME);
      break;
    case 'r':
      ui_sort(UI_SORT_RATE);
      break;
    case 't':
      ui_sort(UI_SORT_TOTAL);
      break;
    case 'l':
      ui_sort(UI_SORT_LINE);
      break;
    case 'p': 
      ui_sort(UI_SORT_TYPE);
      break;
    default:
      break;
    }
    disp_mode = UI_MODE_NORMAL;
    need_redraw = 1;
    break;
  }

  if(need_redraw)
    ui_redraw();
  else
    ui_refresh();
  
  return 0;
}

int
ui_refresh() {

  refresh();
  return 0;
}

int
ui_redraw() {

  switch(disp_mode) {
  case UI_MODE_NORMAL:
    erase();
    header_show();
    list_show();
    break;
  case UI_MODE_HELP:
    ui_clear();
    header_show();
    help_show();
    break;
  }

  refresh();
  return 0;
}
