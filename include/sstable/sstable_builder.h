#ifndef _TABLE_BUILDER_H_
#define _TABLE_BUILDER_H_
#include <fstream>
#include "block_builder.h"
#include "block_handle.h"



class SSTableBuilder{
public:
    static const int MAX_BLOCK_SIZE = 4 * 1024;
    SSTableBuilder(std::string filename);
    SSTableBuilder(const SSTableBuilder&) = delete;
    SSTableBuilder& operator=(const SSTableBuilder&) = delete;

    // REQUIRES: Either Finish() or Abandon() has been called.
    ~SSTableBuilder();
    void Add(const std::string& key,const std::string& value);

    void Flush();

    bool Finish();

    void Abandon();
    uint64_t NumEntries() const;
    uint64_t FileSize() const;
private:
    bool ok() const {};
    void WriteBlock(BlockBuilder* block,BlockHandle* handle);
    
    std::ofstream* file_;
    uint64_t offset_;
    uint64_t num_entries_;
    BlockBuilder* data_block_builder_; 
    BlockBuilder* index_block_builder_;
    bool pending_index_entry_;
    BlockHandle* pending_index_handle_;
    std::string pending_index_key_;
};




#endif