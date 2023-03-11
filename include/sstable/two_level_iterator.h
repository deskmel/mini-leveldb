#ifndef _TWO_LEVEL_ITERATOR_H
#define _TWO_LEVEL_ITERATOR_H
#include <string>
#include "../iterator/iterator.h"
#include "../sstable/block_handle.h"
#include "../sstable/sstable.h"
#include "../utils/status.h"
class SSTable;


Iterator* NewTwoLevelIterator(Iterator* index_iter,const SSTable* table);


#endif