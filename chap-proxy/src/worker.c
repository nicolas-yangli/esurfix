/* 
 * worker.h - CHAP Proxy worker process
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#include "worker.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

int
worker_main(int sockfd, char connect_script[]){
    int fd;

    if((fd = dup2(sockfd, 4) < 0)){
        fprintf(stderr, "Couldn't open file descriptor 4: %m");
        syslog(LOG_ERR, "Couldn't open file descriptor 4: %m");
        exit(EXIT_FAILURE);
    }
    fcntl(fd, F_SETFD, 0);  /*  Not close on exec.  */

    closelog();
    return execl("/bin/sh", "-c", connect_script, NULL);
}
