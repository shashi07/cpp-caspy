#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>
#include <stdint.h>
#include "send_recv.h"
#include "json/json.h"
#include "serialize.h"



class Sender 
{

public :
	Sender(const char *host,const char *port);
	virtual ~Sender();
	bool send(BlockInfo obj);
	bool send(FileInfo obj);
	bool send(const char *msg);
	bool send(const char *msg,uint32_t len);
	int create_socket();

private :
	  struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
  	  struct addrinfo *host_info_list;
  	  int socketfd ;

};

#endif