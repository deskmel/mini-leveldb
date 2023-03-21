#include "../include/sstable/block_handle.h"
#include "../include/utils/code.h"


void BlockHandle::EncodeTo(std::string& dst) const{
    PutFixed32(dst,size_);
    PutFixed32(dst,offset_);
}

bool BlockHandle::DecodeFrom(const std::string& input){
    if (input.size()==8){
        size_ = DecodeFixed32(input);
        offset_ = DecodeFixed32(input.substr(4));
        return true;
    }
    return false;
}

void Footer::EncodeTo(std::string& dst) const{
    metablock_handle_.EncodeTo(dst);
    indexblock_handle_.EncodeTo(dst);
    PutFixed64(dst,kTableMagicNumber);
    uint64_t magic = DecodeFixed64(dst.substr(16,8));
    // printf("%ld %ld\n",magic,kTableMagicNumber);
}

bool Footer::DecodeFrom(const std::string& input){
    std::string magic_str = input.substr(16,8);
    const uint64_t magic_num = DecodeFixed64(magic_str);
    if (magic_num!=kTableMagicNumber){
        // corruption
        return false;
    }
    bool success = metablock_handle_.DecodeFrom(input.substr(0,8));
    if (success){
        success = indexblock_handle_.DecodeFrom(input.substr(8,8));
    }
    return success;

}