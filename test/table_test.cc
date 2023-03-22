#include "../include/sstable/sstable_builder.h"
#include "../include/sstable/sstable.h"
#include "../include/utils/testutils.h"
#include "../include/skiplist/comparator.h"

TEST(TableTest,Basic){
    std::string filename = "./table.ldb";
    SSTableBuilder table_builder(filename);
    table_builder.Add("1234","aa");
    table_builder.Add("2345","bb");
    table_builder.Finish();

    SSTable* table = SSTable::Open(filename);
    Iterator* iter = table->NewIterator();
    iter->Seek("1234");
    ASSERT_TRUE(iter->Valid());
    ASSERT_EQ(iter->value(),"aa");
    iter->Seek("2345");
    ASSERT_TRUE(iter->Valid());
    ASSERT_EQ(iter->value(),"bb");

}