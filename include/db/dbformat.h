#ifndef _DBFORMAT_H_
#define _DBFORMAT_H_

#include <iostream>
#include "../utils/code.h"
// value type
enum ValueType { kTypeDeletion = 0x0, kTypeValue = 0x1 };

static const ValueType kValueTypeForSeek = kTypeValue;

typedef uint64_t SequenceNumber;

static const SequenceNumber kMaxSequenceNumber = ((0x1ull << 56) - 1);


static const int kNumLevels = 7;

// Level-0 compaction is started when we hit this many files.
static const int kL0_CompactionTrigger = 4;

// Soft limit on number of level-0 files.  We slow down writes at this point.
static const int kL0_SlowdownWritesTrigger = 8;

// Maximum number of level-0 files.  We stop writes at this point.
static const int kL0_StopWritesTrigger = 12;

// write buffer size 
static size_t write_buffer_size = 4 * 1024 * 1024;

class LookupKey{
    public:
    LookupKey(const std::string& user_key,SequenceNumber seq);
    LookupKey(const LookupKey&) = delete;
    LookupKey& operator=(const LookupKey&) = delete;

    ~LookupKey();

    std::string memtable_key() const {return key.substr(0,user_key_size+12);};
    std::string internal_key() const {return key.substr(4,user_key_size+8);};
    std::string user_key() const {return key.substr(4,user_key_size);};
    private:
    std::string key;
    int user_key_size;

};

#endif