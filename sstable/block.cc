#include <algorithm>
#include "../include/sstable/block.h"
#include "../include/utils/code.h"
#include "../include/utils/status.h"
class Block::Iter : public Iterator{
    private:
    const Comparator* const comparator_;
    const Block* block_;
    std::vector<InternalKeyEntry>::const_iterator iter_;
    public:
    Iter(const Comparator* cmp,Block* block):block_(block),comparator_(cmp){
        iter_ = block->items_.end();
    }
    ~Iter(){};
    bool Valid()const override{
        return iter_!=block_->items_.end();
    }
    std::string key()const override{
        return iter_->key_;
    }
    std::string value()const override{
        return iter_->value_;
    }
    void Next() override{
        if (Valid()) iter_ = iter_++;
    }
    void Prev() override{
        if (iter_!=block_->items_.begin()){
            iter_-=1;
        }
    }
    void Seek(const std::string& key) override{
        iter_ = lower_bound(block_->items_.begin(),block_->items_.end(),key,
        [](const InternalKeyEntry& entry,const std::string& key){
            return entry.key_<key;
        }
        );
        // printf("iter  %d %s\n",iter_-block_->items_.begin(),iter_->key_.c_str());
    }
    void SeekToFirst() override{
        iter_ = block_->items_.begin();
    }
    void SeekToLast() override{
        iter_ = block_->items_.end()-1;
    }
    Status status() const override{
        return Status::OK();
    }
};
static inline InternalKeyEntry DecodeEntry(int& start_index,const std::string& data){
    std::string keysize_str = data.substr(start_index,4);
    int keysize = DecodeFixed32(data);
    std::string key = data.substr(start_index+4,keysize);
    int valuesize = DecodeFixed32(data.substr(start_index+4+keysize,4));
    std::string value = data.substr(start_index+4+keysize+4,valuesize);
    start_index += keysize+valuesize+8;
    return InternalKeyEntry{key,value};
}

Block::Block(const std::string content):size_(content.size()){
    // just initialize from a string
    int start_index = 0;
    int counter = DecodeFixed32(content.substr(content.size()-4));
    for (int i=0;i<counter;i++){
        InternalKeyEntry entry = DecodeEntry(start_index,content);
        items_.push_back(entry);
    }
}

Block::~Block(){

}
Iterator* Block::NewIterator(const Comparator* cmp){
    return new Iter(cmp,this);
}


