#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include "sha1.h"
#include "client.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "gpuhash.h"
#include "threaddata.h"
#define PACKET_SIZE (4*1024)
#define BUFLEN (500* 1024*1024)

using namespace std;
using namespace sha1;

// Data structure to be passed as an argument to calc threads.

timespec diff(timespec start, timespec end);
/* Thread function to send blocks  to server */
void *send_blocks(void *arg) {
  Send_data_t *blocks = (Send_data_t *)arg;
  cout<<"Inside send block";
  for(int i=0;i<blocks->blocks;i++)
  {
    uint32_t iseq = i;
    uint32_t isize = blocks->data[i].len;
    char hash2[41]="helo";
    toHexString(blocks->data[i].hash,hash2);
    std::string ihash_val = hash2;
    std::string iblock_data;
    if(blocks->data[i].data != NULL)
        iblock_data = blocks->data[i].data;
    else
        iblock_data = "hello";
    BlockInfo obj(iseq, isize, ihash_val,iblock_data);
    blocks->s->send(obj);
    blocks->s->send((const char*)blocks->data[i].data,blocks->data[i].len);
  }
  cout<<"Exiting send function"<<endl;
  blocks->s->send("DONE");
  cout<<"Exiting send function";
  pthread_exit(NULL);
}


/*Thread function to calcuate hash of one block*/

void *calc_hash_thread(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  
  calc(data->data,data->len,data->hash);
 
  pthread_exit(NULL);
}

/*Read file and fill buffer */
int readFile(char* filename, unsigned char * &buffer,int &charRead,int offset=0){

    ifstream fileObject;
    fileObject.open(filename, ios::in|ios::binary);

    if(fileObject.is_open()){

        fileObject.seekg (0, fileObject.end);
        int length = fileObject.tellg();
        fileObject.seekg (0, fileObject.beg);
        int l;
        if( offset >= length )
			return 1;
        if(length-offset >= BUFLEN){
            
            l =BUFLEN;
            //buffer = (unsigned char *)malloc((l)*sizeof(unsigned char));
			buffer = new unsigned char[l];
        }
        else{
			l= length-offset;
			cout<<l<<endl;
            //buffer = (unsigned char *)malloc((l)*sizeof(unsigned char));
            buffer = new unsigned char[l];
        }
        

        fileObject.seekg(offset);
        fileObject.read((char*)buffer,l);
        charRead = fileObject.gcount();
        fileObject.close();

       
        return 0;
    }

        return -1;

}
// return the filename from a given path.
std::string SplitFilename (const std::string& str)
{

  unsigned found = str.find_last_of("/\\");
  return str.substr(found+1);
}


int calc_cpu(unsigned char *data1, int charRead, thread_data_t * &t)
{
    int rc;
    char hash[41];
    int blocks = charRead/(PACKET_SIZE);
        int remainder = charRead % (PACKET_SIZE);
        if ( remainder )
            blocks++;
        t = new thread_data_t[blocks];
        
        int no_of_thread_blocks = blocks/300;
        int remainders = blocks%300;
        if (remainders)
            no_of_thread_blocks++;
        cout<<no_of_thread_blocks <<" " <<remainders<<endl;
        pthread_t *calculator = new pthread_t[blocks];
        
        for(int b=0;b<no_of_thread_blocks;b++){
            int limit = 300;
            if(b==no_of_thread_blocks-1)
                limit = remainders;
            for (int i=b*300;i<(b*300+limit);i++){
                int len = PACKET_SIZE;
                if (i== blocks-1)
                    len = remainder;
                t[i].data = new char[len];
                memcpy ( t[i].data, &data1[i*PACKET_SIZE], len);
                t[i].len = len;
                if ((rc = pthread_create(&calculator[i], NULL, calc_hash_thread, &t[i]))) {
                        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
                        return EXIT_FAILURE;
                    }
            }
              
            for (int i = b*300; i < (b*300+limit); ++i) {
                pthread_join(calculator[i], NULL);
            }
            
        }

        delete[] calculator;
        /*
          for (int i = 0; i < blocks; ++i) {
            toHexString(t[i].hash,hash);
            cout<<i<<" "<<hash<<endl;
        }
        */
        return blocks;
          
}

int main(int argc, char **argv)
{

    if(argc != 3)    
        return 1;
    clock_t start, end;
     double cpu_time_used;

    int file=0;
    if((file=open(argv[2],O_RDONLY)) < -1)
        return 1;
 
    struct stat fileStat;
    if(fstat(file,&fileStat) < 0)    
        return 1;

    close(file);

     //   FileInfo(std:string fname,uint32 isize,uint32 iatime,uint32 imtime, uint32 ictime,uint32 imode,uint32 iuid, uint32 gid);
    unsigned char *data1=NULL;
    int charRead,offset=0,retStatus,rc;
    char hash[41];
    pthread_t calculator, sender;
    Sender s("localhost","28812");
    s.create_socket();
    std::string filename =  argv[2];

    FileInfo f( SplitFilename(filename) , (uint32_t)fileStat.st_size, (uint32_t)fileStat.st_atime,
     (uint32_t)fileStat.st_mtime, (uint32_t)fileStat.st_ctime, (uint32_t)fileStat.st_mode, (uint32_t)fileStat.st_uid, (uint32_t)fileStat.st_gid);
    s.send("POST");
    s.send(f);

    thread_data_t *t;

    while(1){

        retStatus = readFile(argv[2],data1,charRead,offset);
        offset+= charRead;
        if (retStatus == -1 || retStatus == 1)
            break;
        cout<<"aa gaya";
        cout<< charRead<<endl;

        int blocks;
        if (strcmp(argv[1],"-g")==0){
            cout<<"GPU"<<endl;
            blocks = calc_gpu(data1,charRead,t);
        }
        else if (strcmp(argv[1],"-c")==0){
            cout<<"CPU"<<endl;
                timespec time1, time2;

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
            blocks = calc_cpu(data1,charRead,t);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);

        cout<<"Time taken CPU : "<<diff(time1,time2).tv_sec<<"s :"<<diff(time1,time2).tv_nsec<<"ns"<<endl;
        
        }
        else {
            delete[] data1;
            break;

        }
        cout << blocks << endl;
        Send_data_t senddata(blocks,t,&s);
        cout<<"Creating Sender thread"<<endl;

    if ((rc = pthread_create(&sender, NULL, send_blocks, &senddata))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
   
     pthread_join(sender, NULL);
     
     cout<<retStatus;

        delete[] data1; 
        for(int i=0;i<blocks;i++)
        {
                delete[] t[i].data;

        }

        delete[] t;
        if(retStatus==1)
            break;
    }

    printf("Completed");
    return 1;
}

timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}