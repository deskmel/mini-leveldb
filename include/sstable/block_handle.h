#ifndef _BLOCK_HANDLE_H_
#define _BLOCK_HANDLE_H_
#include <string>
// kTableMagicNumber was picked by running
//    echo http://code.google.com/p/leveldb/ | sha1sum
// and taking the leading 64 bits.


static const uint64_t kTableMagicNumber = 0xdb4775248b80fb57ull;

class BlockHandle{
    
    uint32_t size_;
    uint32_t offset_;
    public:
    static const int kBlockHandleSize = 8;
    int offset() const{return offset_;}
    void set_offset(int offset) {offset_ = offset;}

    uint32_t size() const { return size_;}
    void set_size(int size) {size_ = size;}

    void EncodeTo(std::string& dst) const;
    bool DecodeFrom(const std::string& input);
};



// class IndexBlockHandle{
//     InternalKeyEntry* entry;
// }


class Footer{
    
    BlockHandle indexblock_handle_;
    BlockHandle metablock_handle_;
    public:
    static const int kMaxEncodedLength = 2*BlockHandle::kBlockHandleSize+8;
    Footer() = default;
    const BlockHandle& metaindex_handle() const { return metablock_handle_;}
    void set_metaindex_handle(const BlockHandle& h){ metablock_handle_ = h;}

    const BlockHandle& index_handle() const { return indexblock_handle_;}
    void set_index_handle(const BlockHandle& h){indexblock_handle_ = h;}

    void EncodeTo(std::string& dst) const;
    bool DecodeFrom(const std::string& input);
};


#endif