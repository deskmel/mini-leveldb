#ifndef _BLOCK_H_
#define _BLOCK_H_
#include <vector>
#include "../iterator/iterator.h"
#include "../skiplist/comparator.h"

struct InternalKeyEntry{
    std::string key_;
    std::string value_;
};


class Block{
    public:
    explicit Block(const std::string content);
    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;
    ~Block();
    size_t size() const {return size_;}
    Iterator* NewIterator(const Comparator* comparator);

private:
    class Iter;
    std::vector<InternalKeyEntry> items_;
    size_t size_;
};
#endif