#include <cstring>

#include "../include/utils/code.h"
#include "../include/sstable/sstable.h"
#include "../include/skiplist/comparator.h"

SSTable* SSTable::Open(std::string filename){
    std::ifstream* file = new std::ifstream();
    file->open(filename.c_str());
    file->seekg(-Footer::kMaxEncodedLength,std::ios::end);
    char buf[Footer::kMaxEncodedLength];
    file->read(buf,Footer::kMaxEncodedLength);
    Footer footer;
    std::string buff;
    buff.assign(buf,Footer::kMaxEncodedLength);
    // memcpy(&buff[0],buf,Footer::kMaxEncodedLength);
    bool success = footer.DecodeFrom(buff);
    if (!success){
        // panic("Error while reading footer");
    }
    Block* index_block = ReadBlock(&footer.index_handle(),file);
    SSTable* table = new SSTable(index_block,file,footer);
    return table;
}

static std::string GetUserKey(std::string& key){
    return key.substr(0,key.size()-8);
}

static uint64_t GetTag(std::string& key){
    return DecodeFixed64(key.substr(key.size()-8,8));
}

bool SSTable::InternalGet(const LookupKey& l,std::string& value){
    auto iter = NewIterator();
    iter->Seek(l.internal_key());
    if (iter->Valid()){
        std::string entry = iter->key();
        uint32_t key_length;
        if (GetUserKey(entry) == l.user_key()){
            const uint64_t tag = GetTag(entry);
            switch(static_cast<ValueType>(tag & 0xff)){
                case kTypeValue:{
                    value = iter->value();
                    return true;
                }
                case kTypeDeletion:
                return false;
            }
        }
    }
    return false;

}
Block* SSTable::ReadBlock(const BlockHandle* blockhandle) const {
    return ReadBlock(blockhandle,file_);
}
Block* SSTable::ReadBlock(const BlockHandle* blockhandle,std::ifstream* file){
    std::string buf;
    
    file->seekg(blockhandle->offset(),std::ios::beg);
    char * buff = new char [blockhandle->size()];
    file->read(buff,blockhandle->size());
    buf.assign(buff,blockhandle->size());
    delete [] buff;
    Block* block = new Block(buf);
    return block;
}


Iterator* SSTable::NewIterator() const {
    return NewTwoLevelIterator(index_block_->NewIterator(new Comparator()),this);
}

