#ifndef _FILENAME_H_
#define _FILENAME_H_
#include <string>
std::string DescriptorFileName(const std::string& dbname, uint64_t number);

std::string TableFileName(const std::string& dbname, uint64_t number);

std::string SSTTableFileName(const std::string& dbname, uint64_t number);

std::string TempFileName(const std::string& dbname, uint64_t number);

std::string CurrentFileName(const std::string& dbname);
#endif