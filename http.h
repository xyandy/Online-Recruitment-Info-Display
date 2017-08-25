#ifndef _HTTPD_
#define _HTTPD_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<errno.h>
#include<ctype.h>
#include<strings.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include<pthread.h>


#define BUF_SIZE 1024



int start_up(const char* ip, int port);

void* handle_request(void* x);

void not_found(int client);

void echo_www(int sock, char* path, int size);





















#endif


