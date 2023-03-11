#include <cassert>
#include "../include/db/dbformat.h"


static uint64_t PackSequenceAndType(uint64_t seq, ValueType t) {
  assert(seq <= kMaxSequenceNumber);
  assert(t <= kValueTypeForSeek);
  return (seq << 8) | t;
}

LookupKey::LookupKey(const std::string& user_key,SequenceNumber seq){
    key.clear();
    size_t key_size = user_key.size();
    user_key_size = key_size;
    PutFixed32(key,key_size+8);
    
    // key.append(user_key,key_size);
    key += user_key;
    PutFixed64(key,PackSequenceAndType(seq,kValueTypeForSeek));
}


LookupKey::~LookupKey(){}