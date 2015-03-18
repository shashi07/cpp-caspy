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
#include "client.h"


Sender::Sender(const char *host,const char* port){
  int status;
  memset(&host_info, 0, sizeof host_info);
  host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
  host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
  status = getaddrinfo(host, port, &host_info, &host_info_list);
  if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;
  
}

int Sender::create_socket(){
  int status;
  socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, 
  host_info_list->ai_protocol);
  if (socketfd == -1){  
    std::cout << "socket error " ;
    return false;
  }

    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      std::cout << "connect error" ;
      return false;
    }
  return true;
}

Sender::~Sender(){
    freeaddrinfo(host_info_list);
    close(socketfd);
}

bool Sender::send(const char *msg){
  
  uint32_t len = strlen(msg);

  if( send_msg(socketfd,msg,len) < len )
    return false;
  else
    return true;

}

bool Sender::send(const char *msg, uint32_t length){
  

  if( send_msg(socketfd,msg,length) < length )
    return false;
  else
    return true;

}


bool Sender::send(BlockInfo obj){
  std::string output;
  CJsonSerializer::Serialize( &obj, output);
  if( output.length() > send_msg(socketfd,output.c_str(),output.length()) )
    return false;
  return true;  
}


bool Sender::send(FileInfo obj){
  std::string output;
  CJsonSerializer::Serialize( &obj, output);
  if( output.length() > send_msg(socketfd,output.c_str(),output.length()) )
    return false;
  return true;  
}
/*
int main()
{

  int status;
  struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
  struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.
  
  memset(&host_info, 0, sizeof host_info);

  std::cout << "Setting up the structs..."  << std::endl;

  host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
  host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

  // Now fill up the linked list of host_info structs with google's address information.
  status = getaddrinfo("localhost", "28812", &host_info, &host_info_list);
  // getaddrinfo returns 0 on succes, or some other value when an error occured.
  // (translated into human readable text by the gai_gai_strerror function).
  if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;
  
  std::cout << "Creating a socket..."  << std::endl;
  int socketfd ; // The socket descripter
  socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, 
  host_info_list->ai_protocol);
  if (socketfd == -1)  std::cout << "socket error " ;

  std::string name = "shashi.doc";
  FileInfo f(name,320,123456564,324245232,123134,16893,1000,1000);

  std::string output;
  CJsonSerializer::Serialize( &f, output);
 
  std::cout << "testClass Serialized Output\n" << output << "\n\n\n";

  std::cout << "Connect()ing..."  << std::endl;
  status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1)  std::cout << "connect error" ;

  send_msg(socketfd,output.c_str(),output.length());

  freeaddrinfo(host_info_list);
  close(socketfd);
  //   FileInfo(std:string fname,uint32 isize,uint32 iatime,uint32 imtime, uint32 ictime,uint32 imode,uint32 iuid, uint32 gid);


  return 0;
}    
*/