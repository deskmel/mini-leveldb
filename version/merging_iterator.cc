#include "../include/version/version.h"

class MergingIterator : public Iterator{
    public:
    MergingIterator(std::vector<Iterator*> children):children_(children),current_(nullptr){

    }
    ~MergingIterator() override {}
    bool Valid() const override {return (current_!=nullptr);}
    void SeekToFirst() override{
        for (int i=0;i<children_.size();i++){
            children_[i]->SeekToFirst();
        }
        FindSmallest();
        direction_ = kForward;
    }
    void SeekToLast() override{
        for (int i=0;i<children_.size();i++){
            children_[i].SeekToLast();
        }
        FindLargest();
        direction_ = kReverse;
    }

    void Seek(std::string& target) override{
        for (int i = 0; i < children_.size(); i++) {
        children_[i].Seek(target);
        }
        FindSmallest();
        direction_ = kForward;
    }

    void Next() override{
        if (direction_!=kForward){
            for (int i=0;i<children_.size();i++){
                if (children_[i]!=current_){
                    children_[i]->Seek(key());
                    if (children_[i]->Valid()&&children_[i].key()==key()){
                        children_[i]->Next();
                    }
                }
            }
            direction_ = kForward;
        }
        current_->Next();
        FindSmallest();
    }

    void Prev() override{
        if (direction_!=kReverse){
            for (int i=0;i<children_.size();i++){
                if (children_[i]!=current_){
                    children_[i]->Seek(key());
                    if (children_[i]->Valid()){
                        children_[i]->Prev();
                    }else{
                        children_[i]->SeekToLast();
                    }
                }
            }
            direction_ = kReverse;
        }
        current_->Prev();
        FindLargest();
    }
    std::string key() const override{
        return current_->key();
    }
    std::string value() const override{
        return current_->value();
    }
    Status status() const override{
        return Status::OK();
    }
    private:
    void FindSmallest(){
        Iterator* smallest = nullptr;
        for (int i=0;i<children_.size();i++){
            if (children_[i]->Valid()){
                if (smallest==nullptr){
                    smallest = children_[i];
                }else if (children_[i]->key()<smallest->key()){
                    smallest = children_[i];
                }
            }
        }
        current_ = smallest;
    }
    void FindLargest(){
        Iterator* largest = nullptr;
        for (int i=children_.size()-1;i>=0;i--){
            if (children_[i]->Valid()){
                if (largest==nullptr){
                    largest = children_[i];
                }else if (children_[i]->key()>largest->key()){
                    largest = children_[i];
                }
            }
        }
        current_ = largest;
    }
    enum Direction { kForward, kReverse };
    Direction direction_;
    Iterator* current_;
    std::vector<Iterator*> children_;
}

Iterator* NewMergingIterator(std::vector<Iterator*>& children){
    if (n==0){
        return nullptr;
    }else if (n==1){
        return children_[0];
    }else {
        return new MergingIterator(children);
    }
}