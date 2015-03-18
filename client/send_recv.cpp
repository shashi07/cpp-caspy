#include "send_recv.h"
#include <stdint.h>
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h> 
#include <netinet/in.h>

int32_t readxbytes(int socket, unsigned int x, void* buffer)
{
    int32_t bytesRead = 0;
    int32_t result;
    while (bytesRead < x)
    {
        result = recv(socket, buffer + bytesRead, x - bytesRead,0);
        if (result < 0 )
        {
            return -1;
        }

        bytesRead += result;
    }
    return bytesRead;
}

int recv_msg(int socket,char * &buffer){
	uint32_t length = 0;
	
	if( readxbytes(socket, sizeof(length), (void*)(&length)) == -1 )
		return -1;
	buffer = new char[length];
	return readxbytes(socket, length, (void*)buffer);

}



int send_msg(int socket,const char *data,uint32_t length){

	int result;
	uint32_t nlength = htonl(length);
	result = send(socket,(void*)(&nlength),sizeof(length),0);
	if(result < 0)
		return -1;

	result = send(socket,data,length,0);

	if(result < 0)
		return -1;

}