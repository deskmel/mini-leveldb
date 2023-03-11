#include "../include/skiplist/skiplist.h"

SkipList::Iterator::Iterator(const SkipList* list):list_(list){
    node_ = nullptr;
}

bool SkipList::Iterator::Valid() const{
    return node_!=nullptr;
}

void SkipList::Iterator::Next(){
    node_ = node_->Next(0);
}
void SkipList::Iterator::Prev(){
    node_ = list_->FindLessThan(node_->key_);
    if (node_ == list_->head_){
        node_ =nullptr;
    }
}

void SkipList::Iterator::Seek(const std::string& target){
    node_ = list_->FindGreaterOrEqual(target,nullptr);
}

void SkipList::Iterator::SeekToFirst(){
    node_ = list_->head_->Next(0);
}

void SkipList::Iterator::SeekToLast(){
    node_ = list_->FindLast();
    if (node_==list_->head_){
        node_ = nullptr;
    }
}

const std::string& SkipList::Iterator::key() const{
    return node_->key_;
}