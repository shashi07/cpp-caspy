
from socket import *
import time
import pickle
import dbcon as dbconnect
import srfunc
import struct
import json
from thread import *

HOST = 'localhost'
PORT = 28812
BUFSIZE = 1024
ADDR = (HOST, PORT)
tcpTimeSrvrSock = socket(AF_INET,SOCK_STREAM)
tcpTimeSrvrSock.bind(ADDR)
tcpTimeSrvrSock.listen(50)

# data format : (blockdata,hash,block_no,filename,dirstructure,username)



def client_thread(tcpTimeClientSock) :
    request = srfunc.recv_msg(tcpTimeClientSock)
    if request == "POST" :
    
      file_info = srfunc.recv_msg(tcpTimeClientSock)
      file_info_dict = json.loads(file_info)
      print file_info_dict
      inode = dbconnect.create_inode(1,file_info_dict['filename'])
      dbconnect.fill_inode_data(inode=inode,mode=file_info_dict['mode'],uid=file_info_dict['uid'],gid=file_info_dict['gid'],atime=file_info_dict['atime'],mtime=file_info_dict['mtime'],
                      ctime= file_info_dict['ctime'],size=file_info_dict['size'])
      
      con =  dbconnect.opencon(database = 'mysqlfs')
      t0 = time.time()
      while True:
        file_info = srfunc.recv_msg(tcpTimeClientSock)
        if file_info == "DONE" :
          break;
        file_info_dict = json.loads(file_info)
        data = srfunc.recv_msg(tcpTimeClientSock)
        h_id = dbconnect.insert_hash(con=con,hash_val=file_info_dict['hash_val'],size=file_info_dict['size'],data=data)
        dbconnect.insert_in_db_hash(con=con,inode=inode,seq=file_info_dict['seq'],hash_id = h_id)
      dbconnect.closecon(con)
      print time.time() - t0
    tcpTimeClientSock.close()




while True:
  print 'waiting for connection...'
  tcpTimeClientSock, addr = tcpTimeSrvrSock.accept()
  print '...connected from:', addr
  start_new_thread(client_thread,(tcpTimeClientSock,))

tcpTimeSrvrSock.close()

'''
from socket import *
from time import ctime
import pickle
import dbconnect
import srfunc
import struct
import json
import time

HOST = 'localhost'
PORT = 28812
BUFSIZE = 1024
ADDR = (HOST, PORT)
tcpTimeSrvrSock = socket(AF_INET,SOCK_STREAM)
tcpTimeSrvrSock.bind(ADDR)
tcpTimeSrvrSock.listen(50)
# data format : (blockdata,hash,block_no,filename,dirstructure,username)
while True:
  print 'waiting for connection...'
  tcpTimeClientSock, addr = tcpTimeSrvrSock.accept()
  print '...connected from:', addr
  request = srfunc.recv_msg(tcpTimeClientSock)
  if request == "POST" :
    
      file_info = srfunc.recv_msg(tcpTimeClientSock)
      file_info_dict = json.loads(file_info)
      print file_info_dict
      inode = dbconnect.create_inode(1,file_info_dict['filename'])
      
      
      dbconnect.fill_inode_data(inode=inode,mode=file_info_dict['mode'],uid=file_info_dict['uid'],gid=file_info_dict['gid'],atime=file_info_dict['atime'],mtime=file_info_dict['mtime'],
                      ctime= file_info_dict['ctime'],size=file_info_dict['size'])
      while True:
        t0 = time.time()
        file_info = srfunc.recv_msg(tcpTimeClientSock)
        if file_info == "DONE" :
          break;
        file_info_dict = json.loads(file_info)
        hash_val = srfunc.recv_msg(tcpTimeClientSock)
        h_id = -1
        h_id = dbconnect.check_in_hashesbin(hash_val)
        
        data = srfunc.recv_msg(tcpTimeClientSock)
        if h_id == -1 :
          #print 'creating block'
          h_id = dbconnect.insert_in_hashes(hash_val=hash_val,size=file_info_dict['size'],data=0)
          #f= open()
          #print 'block exists using that.'      
#        print len(data)
        #dbconnect.insert_in_data_blocks(inode,seq=file_info_dict['seq'],hash_val= data)
        #dbconnect.insert_in_db_hash(inode,seq=file_info_dict['seq'],hash_id = h_id)
        print time.time() - t0
        
  print "Done"
      
  tcpTimeClientSock.close()

tcpTimeSrvrSock.close()
'''