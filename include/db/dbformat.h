#ifndef _DBFORMAT_H_
#define _DBFORMAT_H_

#include <iostream>
#include "../utils/code.h"
// value type
enum ValueType { kTypeDeletion = 0x0, kTypeValue = 0x1 };

static const ValueType kValueTypeForSeek = kTypeValue;

typedef uint64_t SequenceNumber;

static const SequenceNumber kMaxSequenceNumber = ((0x1ull << 56) - 1);


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