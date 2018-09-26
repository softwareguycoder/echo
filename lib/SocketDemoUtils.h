#ifndef __SOCKETDEMOUTILS_H__
#define __SOCKETDEMOUTILS_H__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr, inet_pton
#include <netdb.h>


#define OK              0		// The server completed successfully
#define ERROR           -1		// The server encountered an error

#define FALSE           0
#define TRUE            1

void free_buffer(void **ppBuffer);
void error_and_close(int sockFd, const char *msg);
void error(const char* msg);

int SocketDemoUtils_createTcpSocket();
int SocketDemoUtils_populateAddrInfo(char *port, char *hostnameOrIp, struct sockaddr_in *addr);
int SocketDemoUtils_bind(int sockFd, struct sockaddr_in *addr);
int SocketDemoUtils_listen(int sockFd);
int SocketDemoUtils_accept(int sockFd, struct sockaddr_in *addr);
int SocketDemoUtils_recv(int sockFd, char **buf);
int SocketDemoUtils_send(int sockFd, const char *buf, int numBytes);
int SocketDemoUtils_getline(int sockFd, char **lineptr, int *total_read);
int SocketDemoUtils_connect(int sockFd, const char *hostnameOrIp, int port);

#endif //__SOCKETDEMOUTILS_H__
