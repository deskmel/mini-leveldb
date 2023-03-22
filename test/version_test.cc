#include "../include/version/version.h"
#include "../include/utils/testutils.h"
#include "../include/db/dbformat.h"
#include "../include/memtable/memtable.h"
TEST(VersionTest,Empty){
    Version *v = Version::New("testdb");
    FileMetaData meta;
    meta.largest = InternalKeyEntry("1","ab");
    meta.smallest = InternalKeyEntry("2","cd");
    v->AddFile(&meta,0);
    LookupKey key("3",0);
    std::string res;
    v->Get(key,res);
    printf("%s\n",res.c_str());
}

TEST(VersionTest,SetAndLoad){
    Version* v = Version::New("test1db");
    MemTable* table = new MemTable(new Comparator());
    table->Add(0,kTypeValue,"aa","bb");
    table->Add(0,kTypeValue,"bb","cc");
    v->WriteLevel0Table(table);
    uint64_t n = v->Save();
    Version* v2 = Version::Load("test1db",n);
    LookupKey key("aa",0);
    std::string res;
    v2->Get(key,res);
    ASSERT_EQ(res,"bb");
    LookupKey key1("bb",0);
    v2->Get(key1,res);
    ASSERT_EQ(res,"cc");
    LookupKey key2("cc",0);
    bool find = v2->Get(key2,res);
    ASSERT_TRUE(!find);
}


