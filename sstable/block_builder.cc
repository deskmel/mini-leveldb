#include "../include/sstable/block_builder.h"
#include "../include/utils/code.h"


BlockBuilder::BlockBuilder(){
    counter_ = 0;
}

void BlockBuilder::Reset(){
    buffer_.clear();
    counter_ = 0;
}

// BlockBuilder::Add(const InternalKeyEntry& entry){
//     counter+=1;
//     PutFixed32(buffer_,entry.key.size());
//     buffer_.append(entry.key);
//     PutFixed32(buffer_,entry.value.size());
//     buffer_.append(entry.value);
// }

void BlockBuilder::Add(const std::string& key,const std::string& value){
    counter_+=1;
    PutFixed32(buffer_,key.size());
    buffer_.append(key);
    PutFixed32(buffer_,value.size());
    buffer_.append(value);
}

std::string BlockBuilder::Finish(){
    PutFixed32(buffer_,counter_);
    return buffer_;
}

size_t BlockBuilder::CurrentSizeEstimate() const {
    return buffer_.size();
}
