#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_
#include <string>
#include <random>

#include "./comparator.h"
#define MAXLEVELOFSKIPLIST 16



struct Node
{
    std::string const key_;
    Node* next_[MAXLEVELOFSKIPLIST];
    explicit Node(const std::string& key);

    Node* Next(int n);
    void SetNext(int n,Node* x);
    /* data */
};





class SkipList
{
private:
    friend struct Node;
    
    int level_;
    Node* const head_;
    int max_height_;
    const Comparator*  compare_;
public:
    class Iterator;
    explicit SkipList(const Comparator* cmp);
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
    void Insert(const std::string& key);
    bool Contains(const std::string& key) const;
    bool KeyIsAfterNode(const std::string& key,Node* node) const;
    inline int GetMaxHeight()const {return max_height_;}
private:
    inline bool Equal(const std::string& a,const std::string& b) const{
        return (compare_->compare(a,b)==0);
    }
    Node* FindGreaterOrEqual(const std::string& key,Node** prev) const;
    Node* FindLessThan(const std::string& key) const;
    Node* FindLast() const;
    Node* NewNode(const std::string& key,int height);
    int RandomHeight();
};

class SkipList::Iterator{
public:
    explicit Iterator(const SkipList* list);
    bool Valid() const;
    void Next();
    void Prev();
    void Seek(const std::string& target);
    void SeekToFirst();
    void SeekToLast();
    const std::string& key() const;
private:
    const SkipList* list_;
    Node* node_;
};
#endif

