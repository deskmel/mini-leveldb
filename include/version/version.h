#ifndef _VERSION_H_
#define _VERSION_H_
#include <algorithm>
#include <fstream>
#include <vector>
#include <mutex>
#include "../sstable/block.h"
// #include "../sstable/sstable.h"
#include "../sstable/sstable_builder.h"
#include "../memtable/memtable.h"
#include "../db/dbformat.h"



struct FileMetaData{
    int allowed_seeks;
    uint64_t number;
    uint64_t file_size;
    InternalKeyEntry smallest;
    InternalKeyEntry largest;
    void EncodeTo(std::string& content);
    void DecodeFrom(std::ifstream* file);
};

class Version{
    
    public:
    static const int kNumLevels = 12;
    static Version* Load(std::string dbname,uint64_t seqNum);
    static Version* New(std::string dbname);
    Version* Copy();
    void AddFile(FileMetaData* meta,int level);
    void WriteLevel0Table(MemTable* imm);
    bool Get(const LookupKey& key,std::string& val);
    void EncodeTo(std::ofstream *file);
    void DecodeFrom(std::string filename);
    uint64_t LastSequence();
    uint64_t Save();
    uint64_t NextFileNumber();
    int NumLevelFiles(int level);
    private:
    int FindFile(int level,const InternalKeyEntry& key);
    uint64_t seqNum_;
    uint64_t next_file_number_;
    std::mutex mu_;
    std::string dbname_;
    std::vector<FileMetaData*> files_[kNumLevels];
    FileMetaData* file_to_compact_;
    int file_to_compact_level_;
    double compaction_score_;
    int compaction_level_;

};


#endif