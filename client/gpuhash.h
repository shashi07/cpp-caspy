#ifndef GPUHASH_H
#define GPUHASH_H

struct thread_data_t {
  char *data;
  int start;
  int len;
  unsigned char hash[20];
} ;

int calc_gpu(unsigned char *data1,int charRead,thread_data_t * &t);

#endif