#ifndef SERIALIZE_H
#define SERIALIZE_H
#include <stdint.h>
#include "json/json.h"
#include <string>

class IJsonSerializable
{
public:
   virtual ~IJsonSerializable( void ) {};
   virtual void Serialize( Json::Value& root ) =0;
   virtual void Deserialize( Json::Value& root) =0;
};

class FileInfo : public IJsonSerializable
{
public:
   FileInfo( void );
   FileInfo(std::string fname,uint32_t isize,uint32_t iatime,uint32_t imtime, uint32_t ictime,uint32_t imode,uint32_t iuid, uint32_t gid);
   virtual ~FileInfo( void );
   virtual void Serialize( Json::Value& root );
   virtual void Deserialize( Json::Value& root);
 
private:
	std::string filename;
	uint32_t size;
	uint32_t atime;
	uint32_t mtime;
	uint32_t ctime;
	uint32_t mode;
	uint32_t uid;
	uint32_t gid; 
};


class BlockInfo : public IJsonSerializable
{
public:
   BlockInfo( void );
   BlockInfo(uint32_t iseq,uint32_t isize, std::string ihash_val,std::string iblock_data);
   virtual ~BlockInfo( void );
   virtual void Serialize( Json::Value& root );
   virtual void Deserialize( Json::Value& root);
private:
   uint32_t seq;
   uint32_t size;
   std::string hash_val;
   std::string block_data;
};


class CJsonSerializer
{
public:
   static bool Serialize( IJsonSerializable* pObj, std::string& output );
   static bool Deserialize( IJsonSerializable* pObj, std::string& input );
 
private:
   CJsonSerializer( void ) {};
};


#endif