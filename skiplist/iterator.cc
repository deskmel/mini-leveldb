#include "../include/skiplist/skiplist.h"

Iterator::Iterator(const SkipList* list):list_(list){
    node_ = nullptr;
}

bool Iterator::Valid() const{
    return node_==nullptr;
}

void Iterator::Next(){
    node_ = node_->Next(0);
}
void Iterator::Prev(){
    node_ = list_->FindLessThan(node_->key_);
    if (node_ == list_->head_){
        node_ =nullptr;
    }
}

void Iterator::Seek(const std::string& target){
    node_ = list_->FindGreaterOrEqual(target,nullptr);
}

void Iterator::SeekToFirst(){
    node_ = list_->head_->Next(0);
}

void Iterator::SeekToLast(){
    node_ = list_->FindLast();
}