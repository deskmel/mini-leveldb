#include "../include/memtable/memtable.h"



static std::string GetLengthPrefixedKey(const std::string& data){
    uint32_t len=DecodeFixed32(data);
    return data.substr(4,len);
}

static std::string GetLengthPrefixedValue(const std::string& data){
    uint32_t key_len = DecodeFixed32(data);
    return data.substr(4+key_len+8+4);
}

static uint64_t GetTag(const std::string& data){
    uint32_t len = DecodeFixed32(data);
    uint64_t tag = DecodeFixed64(data.substr(len+4));
    return tag;
}




int InternalKeyComparator::compare(const std::string& a,const std::string& b) const {
    std::string key1 = GetLengthPrefixedKey(a);
    std::string key2 = GetLengthPrefixedKey(b);
    if (key1==key2){
        auto tag1 = GetTag(a);
        auto tag2 = GetTag(b);
        // printf("key: %s %s, internal key %s %s, tag %d %d\n",a.c_str(),b.c_str(),key1.c_str(),key2.c_str(),tag1,tag2);
        if (tag1 == tag2) return 0;
        if (tag1 > tag2) return -1;
        return 1;
    }
    if (key1>key2) return 1;
    return -1;
}

MemTable::MemTable(const Comparator* cmp)
    : comparator_(cmp),table_(comparator_){}

MemTable::~MemTable(){}




// entry structure
// key size 
// key data
// tag
// value size
// value data
static const std::string EncodeKey(std::string& scratch,const std::string& target){
    scratch.clear();
    PutFixed32(scratch,target.size());
    scratch.append(target.data(),target.size());
    return scratch;
}


// Iterator
class MemTableIterator : public Iterator{
    public:
    explicit MemTableIterator(MemTable::Table* table): iter_(table) {}
    MemTableIterator(const MemTableIterator&) = delete;
    MemTableIterator& operator=(const MemTableIterator&) = delete;
    ~MemTableIterator() override = default;
    bool Valid() const override {return iter_.Valid();}
    void Seek(const std::string& k) override {iter_.Seek(EncodeKey(tmp_,k));}
    void SeekToFirst() override { iter_.SeekToFirst(); }
    void SeekToLast() override { iter_.SeekToLast(); }
    void Next() override { iter_.Next(); }
    void Prev() override { iter_.Prev(); }
    std::string key() const override{
        return GetLengthPrefixedKey(iter_.key());
    }
    std::string value() const override{
        return GetLengthPrefixedValue(iter_.key());
    }
    Status status() const override{
        return Status::OK();
    }
    private:
    MemTable::Table::Iterator iter_;
    std::string tmp_;
};




void MemTable::Add(SequenceNumber seq,ValueType type,const std::string& key,const std::string& value){
    size_t key_size = key.size();
    size_t val_size = value.size();
    size_t internal_key_size = key_size + 8;
    const size_t encoded_len = 4 + internal_key_size + 4 + val_size;
    std::string buf;
    PutFixed32(buf,internal_key_size);
    buf.append(key.data(),key.size());

    PutFixed64(buf,(seq<<8)|type);
    
    PutFixed32(buf,val_size);
    buf.append(value.data(),value.size());

    table_.Insert(buf);
}

Iterator* MemTable::NewIterator(){
    return new MemTableIterator(&table_);
}

bool MemTable::Get(const LookupKey& key,std::string& value,std::string& status){
    
    std::string memkey = key.internal_key();
    Table::Iterator iter(&table_);
    iter.Seek(memkey);
    if (iter.Valid()){
        std::string entry = iter.key();
        uint32_t key_length;
        if (comparator_->compare(entry,key.memtable_key())==0){
            const uint64_t tag = GetTag(iter.key());
            switch(static_cast<ValueType>(tag & 0xff)){
                case kTypeValue:{
                    value = GetLengthPrefixedValue(iter.key());
                    return true;
                }
                case kTypeDeletion:
                return true;
            }
        }
    }
    return false;
}


