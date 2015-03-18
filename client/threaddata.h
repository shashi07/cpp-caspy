#ifndef THREADDATA_H
#define THREADDATA_H
#include "client.h"
#include "gpuhash.h"
/*struct thread_data_t {
  char *data;
  int start;
  int len;
  unsigned char hash[20];
} ;
*/
// Data structure to be passed as an argument to Sender threads.
struct Send_data_t
{

    thread_data_t *data;
    int blocks;
    Sender *s;

    Send_data_t(int iblocks,thread_data_t *d,Sender *t)
    {
        blocks = iblocks;
        data = d;
        s=t;
    }
};

#endif