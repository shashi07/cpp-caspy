from socket import *
from time import ctime
import pickle
import dbconnect
import srfunc
import struct
import json

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
        file_info = srfunc.recv_msg(tcpTimeClientSock)
        if file_info == "DONE" :
          break;
        file_info_dict = json.loads(file_info)
        data = srfunc.recv_msg(tcpTimeClientSock)
        print len(data)
        dbconnect.insert_in_data_blocks(inode,seq=file_info_dict['seq'],hash_val= data)
        print file_info_dict['seq']
      
  tcpTimeClientSock.close()

tcpTimeSrvrSock.close()