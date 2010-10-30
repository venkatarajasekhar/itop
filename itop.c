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
 * $Id: itop.c,v 1.8 2008-07-29 00:46:11 kos Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#if HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "itop.h"
#include "collect.h"
#include "llog.h"
#include "cursesui.h"

int_collect_t *ic;
int running = 0;

RETSIGTYPE
signal_handler(int sig) {
  switch(sig){
  case SIGINT:
    running = 0;
    break;
  case SIGALRM:
    //alarm(gs->delay);
    //pps_update_all(gs);
    /* fall through */
  case SIGWINCH:
  case SIGCONT:
    ui_redraw();
    break;
  }
  signal(sig, signal_handler);
}

int
itop_run(void) {
  fd_set fds;
  struct timeval tmout;

  running = 1;
  ui_init();
  ic = collect_new();
  signal(SIGINT, signal_handler);

  while(running) {

    collect_update(ic);
    ui_redraw();

    FD_ZERO(&fds);
    FD_SET(0, &fds);
    tmout.tv_sec = 1;
    tmout.tv_usec = 0;
    if(select(2, &fds, NULL, NULL, &tmout) < 0) {
      switch(errno) {
      case EINTR:
	continue;
      default:
	running = 0;
	break;
      }
    }
    
    if(FD_ISSET(0, &fds)) {
      ui_process_events();
      continue;
    }
  }
  
  ui_finish();
  collect_destroy(ic);
  return 0;
}

void
usage(char *argv0, int code) {
  fprintf(stderr, "Usage: %s [-V]\n", argv0);
  fprintf(stderr, "\t-V            display version number and exit\n");
  exit(code);
}

int
main(int argc, char **argv) {
  char c;
  int verbose;
  
  /* defaults */
  verbose = 1;
  
  while((c = getopt(argc, argv, "Vv:")) != EOF) {
    switch(c) {
    case 'V':
      printf("%s %s\n", PACKAGE, VERSION);
      break;
    case 'v':
      verbose = atoi(optarg);
      break;
    case '?':
      usage(argv[0], 1);
      break;
    }
  }

  llog_init(LLOG_STDERR);
  llog_level(verbose);

  itop_run();
  llog_close();
  exit(0);
}

