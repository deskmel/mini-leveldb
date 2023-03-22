#include "../include/version/version.h"

class Version::LevelFileNumIterator : public Iterator{
    public:
    LevelFileNumIterator(const std::vector<FileMetaData*>* flist)
    :flist_(flist),index_(flist->size()){

    }
    bool Valid() const override {return index_ <flist_->size();}
    void Seek(const std::string& target) override{
        index_ = FindFile(*flist_,target);
    }
    void SeekToFirst() override { index_ = 0;}
    void SeekToLast() override{
        index_ = flist_->empty()? 0 : flist_->size()-1;
    }
    void Next() override{
        index_++;
    }
    void Prev() override{
        if (index_==0){
            index = flist_->size();
        }else {
            index_--;
        }
    }
    std::string key() const override{
        return (*flist_)[index_]->largest.key_;
    }
    std::string value() const override {
        std::string buf;
        PutFixed64(buf,(*flist_)[index_]->number);
        PutFixed64(buf,(*flist_)[index_]->file_size);
        return buf;
    }
Status status() const override { return Status::OK(); }
    private:
    const InternalKeyComparator icmp_;
    const std::vector<FileMetaData*>* const flist_;
    uint32_t index_;
}