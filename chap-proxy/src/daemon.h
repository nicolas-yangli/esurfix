/* 
 * daemon.h - implement of daemonizing
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#ifndef DAEMON_H
#define DAEMON_H

int daemonize();
int set_signal_handler(int signo, void (*handler)(int));

#endif  /*  DAEMON_H    */
