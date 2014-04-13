/* 
 * main.c - CHAP Proxy main module
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <signal.h> 
#include <syslog.h>

#include "daemon.h"
#include "worker.h"

void sig_child(int signo);

int
main(int argc, char *argv[]){
    int detach = 0;
    char *listen = "/var/run/chap-proxy/socket";
    char *pidfile = "/var/run/chap-proxy/pidfile";
    char *connect_script = NULL;
    int listenfd;
    int sockfd;
    int pidfd;
    int pid;
    int opt;

    while((opt = getopt(argc, argv, "l:c:dp:")) != -1){
        switch(opt){
            case 'l':
                listen = strdup(optarg);
                break;
            case 'c':
                connect_script = strdup(optarg);
                break;
            case 'd':
                detach = 1;
                break;
            case 'p':
                pidfile = strdup(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-d] [-l LISTEN_SOCKET]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    listenfd = open_listen_socket(listen);

    if(detach)
        daemonize();

    openlog("chap-proxy", LOG_PID, LOG_DAEMON);

    if(pidfile){
        struct flock pidfl;
        char buf[64];
        pidfl.l_type = F_WRLCK;
        pidfl.l_whence = SEEK_SET;
        pidfl.l_start = 0;
        pidfl.l_len = 0;
        if((pidfd = open(pidfile, O_RDWR|O_CREAT, 0600)) < 0){
            fprintf(stderr, "Couldn't open pidfile %s", pidfile);
            syslog(LOG_CRIT, "Couldn't open pidfile %s", pidfile);
            exit(EXIT_FAILURE);
        }
        if(fcntl(pidfd, F_SETLK, &pidfl) < 0){
            fprintf(stderr, "Couldn't lock pidfile %s", pidfile);
            syslog(LOG_CRIT, "Couldn't lock pidfile %s", pidfile);
            exit(EXIT_FAILURE);
        }
        ftruncate(pidfd, 0);
        snprintf(buf, sizeof(buf), "%lu\n", (unsigned long) getpid());
        write(pidfd, buf, strlen(buf));
    }

    signal(SIGCHLD, sig_child);
    for(;;){
        if((sockfd = accept(listenfd, NULL, NULL)) < 0){
            switch(errno){
                case EINTR:
                    continue;
                    break;
                default:
                    fprintf(stderr, "accept: %m");
                    syslog(LOG_CRIT, "accept: %m");
                    exit(EXIT_FAILURE);
            }
        }
        if((pid = fork()) < 0){
            fprintf(stderr, "Couldn't fork: %m");
            syslog(LOG_CRIT, "Couldn't fork: %m");
            exit(EXIT_FAILURE);
        }else if(pid == 0){
            close(listenfd);
            close(pidfd);
            closelog();
            openlog("chap-proxy", LOG_PID, LOG_DAEMON);
            return worker_main(sockfd, connect_script);
        }
    }
}

int
open_listen_socket(char *pathname){
    int sockfd;
    struct sockaddr_un addr;

    if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0 ){
        fprintf(stderr, "socket: %m");
        exit(EXIT_FAILURE);
    }
    unlink(pathname);   /*  ignore errors   */

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, pathname, sizeof(addr.sun_path)-1);
    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
        fprintf(stderr, "bind: %m");
        exit(EXIT_FAILURE);
    }

    if(listen(sockfd, 5) != 0){
        fprintf(stderr, "listen: %m");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void
sig_child(int signo){
    int pid;
    int ret;
    pid = wait(&ret);
}
