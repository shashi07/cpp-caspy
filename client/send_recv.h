#ifndef SENDRECV_H
#define SENDRECV_H
#include <stdint.h>
int readxbytes(int socket, unsigned int x, void* buffer);

int recv_msg(int socket,void * &buffer);

int send_msg(int socket,const char *data,uint32_t length);

#endif