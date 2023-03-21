#ifndef _SSTABLE_H_
#define _SSTABLE_H_
#include <fstream>
#include "../db/dbformat.h"
#include "../iterator/iterator.h"
#include "block.h"
#include "block_handle.h"
#include "block_builder.h"
#include "two_level_iterator.h"
class Block;
class BlockHandle;
class Footer;

class SSTable{
public:

    static SSTable* Open(std::string filename);
    static Block* ReadBlock(const BlockHandle* blockhandle,std::ifstream* file);
    Block* ReadBlock(const BlockHandle* blockhandle) const; 
    SSTable(const SSTable&) = delete;
    SSTable& operator=(const SSTable&) = delete;
    ~SSTable();
    Iterator* NewIterator() const;
    uint64_t ApproximateOffsetOf(const std::string& key) const;
    bool InternalGet(const LookupKey& l,std::string& value);
    
private:
    explicit SSTable(Block* index_block,std::ifstream* file,Footer footer):index_block_(index_block),file_(file),footer_(footer){}
    void ReadMeta(const Footer& footer);
    
    Block* index_block_;
    Footer footer_;
    std::ifstream* file_;
    
};

#endif