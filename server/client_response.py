from socket import *
import hashlib,pickle,srfunc

username = raw_input("Username :")
password = raw_input("Password :")

if username == "shashi" and password=="dexter88" :
        
    HOST = 'localhost'
    PORT = 28812
    BUFSIZE = 1024
    ADDR = (HOST, PORT)
    tcpTimeClientSock = socket(AF_INET, SOCK_STREAM)
    tcpTimeClientSock.connect(ADDR)
    response = raw_input("response :")
    tcpTimeClientSock.send(response)
    if response == "POST" :
        fpath = raw_input("filename:")
        fpath2 = fpath.rsplit('/',1)
        f = open(fpath,"rb")
        i=0
        while True :
          data = f.read(100)
          a = hashlib.sha1()
          if not data:
              break
          a.update(data)
          if len(fpath2) == 1 :
            dataTosend = (data,a.hexdigest(),i,fpath2[0],None,username)
          else :
            dataTosend = (data,a.hexdigest(),i,fpath2[1],fpath2[0],username)
          i=i+1
          tcpTimeClientSock.send(pickle.dumps(dataTosend))
          data = tcpTimeClientSock.recv(BUFSIZE)
          if not data:
              break
          print data
    elif response == "GET" :   #retrieval code 
      while True :
        fpath = raw_input("filename:")
        if not fpath:
            break
        tcpTimeClientSock.send(fpath)
        fdata = ""
        while True :
          #data = tcpTimeClientSock.recv(BUFSIZE)
          data = srfunc.recv_msg(tcpTimeClientSock)
          fdata = fdata +data
          #end = tcpTimeClientSock.recv(BUFSIZE)
          end = srfunc.recv_msg(tcpTimeClientSock)
          print len(end),end + "------------------------------------------------------"
          if end == "True" :
            print fdata
            print "aa gaya"
            f2 = open("retFile/"+fpath,"wb")
            f2.write(fdata)
            f2.close()
            break;
    tcpTimeClientSock.close()
else :
    print "Wrong Username or password"
