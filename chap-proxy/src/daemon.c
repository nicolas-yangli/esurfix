/* 
 * daemon.c - implement of daemonizing
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "daemon.h"

int
daemonize(){
    int pid;
    int fd;

    if((pid = fork()) < 0){
        fprintf(stderr, "Couldn't daemonize (fork failed: %m)");
        exit(EXIT_FAILURE);
    }else if(pid > 0){
        exit(EXIT_SUCCESS);
    }

    setsid();
    set_signal_handler(SIGHUP, SIG_IGN);
    if((pid = fork()) < 0){     /* fork to not be a session leader */
        fprintf(stderr, "Couldn't daemonize (fork failed: %m)");
        exit(EXIT_FAILURE);
    }else if(pid > 0){
        exit(EXIT_SUCCESS);
    }

    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    if((fd = open("/dev/null", O_RDWR)) < 0){
        return 0;
    }else{
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }

    return 0;
}

int
set_signal_handler(int signo, void (*handler)(int)){
    struct sigaction sa;

    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    return sigaction(signo, &sa, NULL);
}
