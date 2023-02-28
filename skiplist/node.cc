#include <string.h>

#include "../include/skiplist/skiplist.h"


Node::Node(const std::string& key):key_(key){
    memset(next_,0,sizeof(next_));
}

Node* Node::Next(int level){
    return next_[level];
}

void Node::SetNext(int level,Node* x){
    next_[level]=x;
}