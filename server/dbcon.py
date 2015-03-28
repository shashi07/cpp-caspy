import MySQLdb as mdb
import sys

def runquery(query,insert=False) :
    
    try :
	# host,db_username,db_password,dbname
        #con = d=mdb.connect('localhost','caspy','caspy','caspy')
        con = mdb.connect('localhost','root','dexter88','mysqlfs')
        cur = con.cursor()
        #print query
        cur.execute(query)
        if insert :
        	a = cur.lastrowid
        else :
        	a = cur.fetchall()
    except mdb.Error, e :
        a = ()
        print e
    finally :
        if con :
            con.commit()
            con.close()
    return a



def opencon(hostname='localhost',db_user='root',db_password='dexter88',database='intmysql'):
    try :
        con = mdb.connect(hostname,db_user,db_password,database)
    except mdb.Error, e :
        raise e
    return con

def query_run(con,query,*argv) :
    cur = con.cursor()
    param = [arg for arg in argv]
    param = tuple(param)
    try :
        cur.execute(query,param)
        a = cur.fetchall()
    except mdb.Error,e :
        a = ()
    cur.close()
    return a


def closecon(con):
    try:
        con.commit()
        con.close()
    except mdb.Error, e:
        raise e

def insert_hash(con,hash_val,size,data) :
    query = """insert IGNORE into hashes(hash,size,data) values (%s,%s,%s);"""
    cur = con.cursor()
    try:
        cur.execute(query,(hash_val,size,data))
        if cur.lastrowid:
            a = cur.lastrowid
        else :
            query = """select hash_id from hashes where hash = %s;"""
            cur.execute(query,(hash_val,))
            a=cur.fetchall()
            a = a[0][0]
    except mdb.Error,e:
        raise e
    cur.close()
    return a




def create_inode(parent,name):
	query = """insert into tree(parent,name) values (%d,'%s');"""%(parent,name)
	return runquery(query,True)
    
def fill_inode_data(inode,mode,uid,gid,atime,mtime,ctime,size):
    query = """insert into inodes(inode,mode,uid,gid,atime,mtime,ctime,size) values (%d,%d,%d,%d,%d,%d,%d,%d);"""%(inode,mode,uid,gid,atime,mtime,ctime,size)
    return runquery(query)

def insert_in_db_hash(con,inode,seq,hash_id):
    query = """insert into data_blocks(inode,seq,hash_id) values (%s,%s,%s);"""
    return query_run(con,query,int(inode),int(seq),hash_id)

