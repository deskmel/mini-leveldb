#ifndef _CODE_H_
#define _CODE_H_
#include <string>
void EncodeFixed32(std::string& buffer,uint32_t value);
uint32_t DecodeFixed32(const std::string& buffer);
void EncodeFixed64(std::string& buffer,uint64_t value);
uint64_t DecodeFixed64(const std::string& buffer);
void PutFixed32(std::string& dst, uint32_t value);
void PutFixed64(std::string& dst, uint64_t value);

#endif