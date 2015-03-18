#include "serialize.h"


FileInfo::FileInfo(std::string fname,uint32_t isize,uint32_t iatime,uint32_t imtime, uint32_t ictime,uint32_t imode,uint32_t iuid, uint32_t igid)
{
	filename = fname;
	size = isize;
	atime = iatime;
	mtime = imtime;
	ctime = ictime;
	mode = imode;
	uid = iuid;
	gid = igid;

}

FileInfo::~FileInfo()
{


}

void FileInfo::Serialize( Json::Value& root )
{
   // serialize primitives
   root["filename"] = filename;
   root["size"] = size;
   root["atime"] = atime;
   root["mtime"] = mtime;
   root["ctime"] = ctime;
   root["mode"] = mode;
   root["uid"] = uid;
   root["gid"] = gid;
}
 
void FileInfo::Deserialize( Json::Value& root )
{
   // deserialize primitives
 	
 	filename = root.get("filename","").asString();;
	size = root.get("size",0).asInt();
	atime = root.get("atime",0).asInt();
	mtime = root.get("mtime",0).asInt();
	ctime = root.get("ctime",0).asInt();
	mode = root.get("mode",0).asInt();
	uid = root.get("uid",0).asInt();
	gid = root.get("gid",0).asInt();
}

BlockInfo::BlockInfo(uint32_t iseq,uint32_t isize, std::string ihash_val,std::string iblock_data)
{
	seq = iseq;
	size = isize;
	hash_val = ihash_val;
	block_data = iblock_data;

}

BlockInfo::~BlockInfo()
{
	
}

void BlockInfo::Serialize( Json::Value& root )
{
   // serialize primitives
   root["seq"] = seq;
   root["size"] = size;
   root["hash_val"] = hash_val;
   //root["block_data"] = block_data;

}
 
void BlockInfo::Deserialize( Json::Value& root )
{
   
}


bool CJsonSerializer::Serialize( IJsonSerializable* pObj, std::string& output )
{
   if (pObj == NULL)
      return false;
 
   Json::Value serializeRoot;
   pObj->Serialize(serializeRoot);
 
   Json::StyledWriter writer;
   output = writer.write( serializeRoot );
 
   return true;
}
 
bool CJsonSerializer::Deserialize( IJsonSerializable* pObj, std::string& input )
{
   if (pObj == NULL)
      return false;
 
   Json::Value deserializeRoot;
   Json::Reader reader;
 
   if ( !reader.parse(input, deserializeRoot) )
      return false;
 
   pObj->Deserialize(deserializeRoot);
 
   return true;
}
