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

#define PACKET_SIZE 4*1024
#define BUFLEN 500* PACKET_SIZE

using namespace std;
using namespace sha1;

// Data structure to be passed as an argument to calc threads.
struct thread_data_t {
  char *data;
  int start;
  int len;
  unsigned char hash[20];
} ;

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
  blocks->s->send("DONE");
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

			buffer[0]='S';
			buffer[l-1] = 'B';
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

int main(int argc, char **argv)
{

    if(argc != 2)    
        return 1;

    int file=0;
    if((file=open(argv[1],O_RDONLY)) < -1)
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
    std::string filename =  argv[1];

    FileInfo f( SplitFilename(filename) , (uint32_t)fileStat.st_size, (uint32_t)fileStat.st_atime,
     (uint32_t)fileStat.st_mtime, (uint32_t)fileStat.st_ctime, (uint32_t)fileStat.st_mode, (uint32_t)fileStat.st_uid, (uint32_t)fileStat.st_gid);
    s.send("POST");
    s.send(f);


    while(1){

        retStatus = readFile(argv[1],data1,charRead,offset);
        offset+= charRead;
        if (retStatus == -1 || retStatus == 1)
            break;
        cout<<"aa gaya";
        int blocks = charRead/(PACKET_SIZE);
        int remainder = charRead % (PACKET_SIZE);
        if ( remainder )
            blocks++;
        thread_data_t *t = new thread_data_t[blocks];
        pthread_t *calculator = new pthread_t[blocks];
        for (int i=0;i<blocks;i++){
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
		  for (int i = 0; i < blocks; ++i) {
			pthread_join(calculator[i], NULL);
		}
		
		
		  for (int i = 0; i < blocks; ++i) {
			toHexString(t[i].hash,hash);
			cout<<i<<" "<<hash<<endl;
		}
		
		delete data1;

    cout<<"Creating Sender thread"<<endl;
   
    Send_data_t senddata(blocks,t,&s);

    if ((rc = pthread_create(&sender, NULL, send_blocks, &senddata))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }

    pthread_join(sender, NULL);
        if(retStatus==1)
            break;
    }

    printf("Completed");
    return 1;
}
