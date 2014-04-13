/* 
 * worker.h - CHAP Proxy worker process
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#ifndef WORKER_H
#define WORKER_H

int worker_main(int sockfd, char connect_script[]);

#endif /*   WORKER_H    */
