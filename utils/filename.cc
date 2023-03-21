#include "../include/utils/filename.h"
#include "../include/utils/code.h"
std::string MakeFileName(const std::string dbname,uint64_t number,std::string subfix){
  char buf[100];
  std::snprintf(buf,sizeof(buf),"%s%06llu.%s",dbname.c_str(),number,subfix.c_str());
  return std::string(buf);
}

std::string DescriptorFileName(const std::string& dbname, uint64_t number) {
  // assert(number > 0);
  char buf[100];
  std::snprintf(buf, sizeof(buf), "MANIFEST-%s%06llu",
                dbname.c_str(),number);
  return std::string(buf);
}

std::string SSTTableFileName(const std::string& dbname, uint64_t number) {
  // assert(number > 0);
  return MakeFileName(dbname, number, "sst");
}

std::string TableFileName(const std::string& dbname, uint64_t number) {
  // assert(number > 0);
  return MakeFileName(dbname, number, "ldb");
}


std::string TempFileName(const std::string& dbname,uint64_t number){
  return MakeFileName(dbname,number,"dbtmp");
}

std::string CurrentFileName(const std::string& dbname){
  return dbname+".current";
}