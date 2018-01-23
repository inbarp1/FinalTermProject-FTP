#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#ifndef NETWORKING_H
#define NETWORKING_H

#define BUFFER_SIZE 256
#define PORT "9002"
#define TEST_IP "127.0.0.1"
//#define CHECK_PASSWD 27
//#define CREAT_ACCONT 28

void error_check(int i, char *s);
int server_setup();
int server_connect(int sd);
int client_setup(char * server);

int forking_server();

void client(char *);

void handle_error();


#endif