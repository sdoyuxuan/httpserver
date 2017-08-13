#ifndef _HTTPD_H_
#define _HTTPD_H_

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 10
#define SIZE 4096

int get_line(int sock,char*buff,int size);
int startup(const char*ip,int port);
void* handler_request(void*arg);
void echo_www(int sock,char*path,int len);
void echo_err(int sock);
#endif
