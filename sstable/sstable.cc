#include "../include/sstable/sstable.h"
#include "../include/skiplist/comparator.h"
SSTable* SSTable::Open(std::string filename){
    std::ifstream* file = new std::ifstream();
    file->open(filename.c_str());
    file->seekg(-Footer::kMaxEncodedLength,std::ios::end);
    char buf[Footer::kMaxEncodedLength];
    file->read(buf,Footer::kMaxEncodedLength);
    Footer footer;
    bool success = footer.DecodeFrom(std::string(buf));
    if (!success){
        // panic("Error while reading footer");
    }
    Block* index_block = ReadBlock(&footer.index_handle(),file);
    SSTable* table = new SSTable(index_block,file,footer);
    return table;
}
Block* SSTable::ReadBlock(const BlockHandle* blockhandle) const {
    return ReadBlock(blockhandle,file_);
}
Block* SSTable::ReadBlock(const BlockHandle* blockhandle,std::ifstream* file){
    std::string buf;
    
    file->seekg(blockhandle->offset());
    char * buff = new char [blockhandle->size()];
    file->read(buff,blockhandle->size());
    buf = std::string(buff);
    delete [] buff;
    Block* block = new Block(buf);
    return block;
}


Iterator* SSTable::NewIterator() const {
    return NewTwoLevelIterator(index_block_->NewIterator(new Comparator()),this);
}

