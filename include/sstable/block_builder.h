#ifndef _BLOCK_BUILDER_H
#define _BLOCK_BUILDER_H
#include <string>
class BlockBuilder{
    public:
    explicit BlockBuilder();
    BlockBuilder(const BlockBuilder&) = delete;
    BlockBuilder& operator=(const BlockBuilder&) = delete;
    void Reset();
    // void Add(const InternalKeyEntry& entry);
    void Add(const std::string& key,const std::string& value);
    std::string Finish();
    size_t CurrentSizeEstimate() const;
    bool empty() {return buffer_.empty();}
    private:
    std::string buffer_;
    int counter_;

};
#endif