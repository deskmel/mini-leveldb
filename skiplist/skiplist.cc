#include "../include/skiplist/skiplist.h"

SkipList::SkipList(Comparator cmp)
:   compare_(cmp),
    head_(NewNode(0,1)),
    max_height_(1){
        for (int i=0;i<MAXLEVELOFSKIPLIST;i++){
            head_->SetNext(i,nullptr);
        }
    }

Node* SkipList::NewNode(const std::string& key,int height){
    return new Node(key);
}

int SkipList::RandomHeight(){
    const int kBranching = 4;
    int height = 1;
    while(height<MAXLEVELOFSKIPLIST&&(rand()%kBranching==0)){
        height++;
    }
    return height;
}


void SkipList::Insert(const std::string& key){

    Node* prev[MAXLEVELOFSKIPLIST];
    Node* x = FindGreaterOrEqual(key,prev);
    
    int height = RandomHeight();
    if (height>GetMaxHeight()){
        for (int i = GetMaxHeight();i<height;i++){
            prev[i] = head_;
        }
        max_height_=height;
    }
    x = NewNode(key,height);
    for (int i=0;i<height;i++){
        x->SetNext(i,prev[i]->Next(i));
        prev[i]->SetNext(i,x);
    }
}

bool SkipList::Contains(const std::string& key) const{
    Node* x = FindGreaterOrEqual(key,nullptr);
    if (x!=nullptr && Equal(key,x->key_)){
        return true;
    }
    return false;

}


bool SkipList::KeyIsAfterNode(const std::string& key,Node* node) const{
    return (node!=nullptr)&&(compare_.compare(node->key_,key)<0);
}

Node* SkipList::FindGreaterOrEqual(const std::string& key,Node** prev) const{
    int maxlevel = GetMaxHeight() -1;
    Node* x = head_,*next;
    for (int level = maxlevel;level>=0;level--){
        next = x->Next(level);
        while (KeyIsAfterNode(key,next)){
            x = next;
            next = x->Next(level);
        }
        if (prev!=nullptr) prev[level] = x;
    }
    return next;
}


Node* SkipList::FindLessThan(const std::string& key) const{
    int maxlevel = GetMaxHeight()-1;
    Node* x=head_,*next;
    for (int level=maxlevel;level>=0;level--){
        next = x->Next(level);
        while (KeyIsAfterNode(key,next)){
            x = next;
            next = x->Next(level);
        }
    }
    return x;
}


Node* SkipList::FindLast() const{
    int maxlevel = GetMaxHeight() - 1;
    Node * x=head_,*next;
    for (int level = maxlevel;level>=0;level--){
        next = x->Next(level);
        while (next!=nullptr){
            x=next;
            next = x->Next(level);
        }
    }
    return x;
}
