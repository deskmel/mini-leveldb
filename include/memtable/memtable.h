#ifndef _MEMTABLE_H_
#define _MEMTABLE_H_


#include "../db/dbformat.h"
#include "../skiplist/skiplist.h"
#include "../skiplist/comparator.h"
#include "../iterator/iterator.h"

class MemTableIterator;
class MemTableBackwardIterator;
class InternalKeyComparator: public Comparator{
    public:
    InternalKeyComparator(){};
    ~InternalKeyComparator(){};
    int compare(const std::string& a,const std::string& b) const;
};

class MemTable{
    public:
    explicit MemTable(const Comparator* cmp);
    MemTable(const MemTable&) = delete;
    MemTable& operator=(const MemTable&) = delete;

    size_t ApproximateMemoryUsage();

    Iterator* NewIterator();

    void Add(SequenceNumber seq,ValueType type,const std::string& key,const std::string& value);

    bool Get(const LookupKey& key,std::string& value,std::string& status);
    ~MemTable();
private:
friend class MemTableIterator;
friend class MemTableBackwardIterator;

typedef SkipList Table;
const Comparator* comparator_;
Table table_;
size_t size_;
};



#endif