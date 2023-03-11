#include "../include/sstable/two_level_iterator.h"

class TwoLevelIterator:public Iterator{
    public:
    TwoLevelIterator(Iterator* index_iter,const SSTable* table);
    ~TwoLevelIterator() override;

    void Seek(const std::string& target) override;
    void SeekToFirst() override;
    void SeekToLast() override;
    void Next() override;
    void Prev() override;

    bool Valid() const override {
        return data_iter_->Valid();
    }

    std::string key() const override{
        return data_iter_->key();
    }
    std::string value() const override{
        return data_iter_->value();
    }
    Status status() const {
        return Status::OK();
    }
    private:
    void SkipEmptyDataBlocksBackward();
    void SkipEmptyDataBlocksForward();
    void SetDataIterator(Iterator* data_iter);
    void InitDataBlock();

    Iterator* index_iter_;
    Iterator* data_iter_;
    std::string data_block_handle_;
    const SSTable* table_;

};

TwoLevelIterator::TwoLevelIterator(Iterator* index_iter,const SSTable* table):index_iter_(index_iter),table_(table){}
TwoLevelIterator::~TwoLevelIterator(){};
void TwoLevelIterator::Seek(const std::string& target)  {
    index_iter_->Seek(target);
    InitDataBlock();
    if (data_iter_!=nullptr) data_iter_->Seek(target);
    SkipEmptyDataBlocksForward();
}

void TwoLevelIterator::SeekToFirst()  {
    index_iter_->SeekToFirst();
    InitDataBlock();
    if (data_iter_!=nullptr) data_iter_->SeekToFirst();
    SkipEmptyDataBlocksForward();
}

void TwoLevelIterator::SeekToLast() {
    index_iter_->SeekToLast();
    InitDataBlock();
    if (data_iter_!=nullptr) data_iter_->SeekToLast();
    SkipEmptyDataBlocksBackward();
}

void TwoLevelIterator::Prev(){
//   assert(Valid());
  data_iter_->Prev();
  SkipEmptyDataBlocksBackward();
}
void TwoLevelIterator::Next(){
    data_iter_->Next();
    SkipEmptyDataBlocksForward();
}

void TwoLevelIterator::SkipEmptyDataBlocksForward(){
    while (data_iter_ == nullptr || !data_iter_->Valid()){
        if (!data_iter_->Valid()){
            SetDataIterator(nullptr);
            return;
        }
        index_iter_->Next();
        InitDataBlock();
        if (data_iter_!=nullptr) data_iter_->SeekToFirst();
    }
}


void TwoLevelIterator::SkipEmptyDataBlocksBackward(){
    while (data_iter_ == nullptr || !data_iter_->Valid()){
        if (!index_iter_->Valid()){
            SetDataIterator(nullptr);
            return;
        }
        index_iter_->Prev();
        InitDataBlock();
        if (data_iter_!=nullptr) data_iter_->SeekToLast();
    }
}

void TwoLevelIterator::SetDataIterator(Iterator* data_iter){
    data_iter_ = data_iter;
}


void TwoLevelIterator::InitDataBlock(){
    if (!index_iter_->Valid()){
        SetDataIterator(nullptr);
    }else{
        std::string handle = index_iter_->value();

        if (data_iter_!=nullptr && data_block_handle_ == handle){
            // do nothing
        }else{
            BlockHandle blockhandle;
            blockhandle.DecodeFrom(handle);
            Iterator* iter = table_->ReadBlock(&blockhandle)->NewIterator(new Comparator());
            SetDataIterator(iter);
        }
    }
}

Iterator* NewTwoLevelIterator(Iterator* index_iter,const SSTable* table){
    return new TwoLevelIterator(index_iter,table);
}