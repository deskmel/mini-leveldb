#include "../include/utils/testutils.h"
#include "../include/sstable/block.h"
#include "../include/sstable/block_builder.h"
#include "../include/skiplist/comparator.h"
TEST(BlockTest,Empty){
    BlockBuilder block_builder;
    Comparator cmp;
    std::string raw = block_builder.Finish();
    Block block(raw);
    auto iter = block.NewIterator(&cmp);
    iter->Seek("1234");
    ASSERT_TRUE(!iter->Valid());
    iter->Seek("");
    ASSERT_TRUE(!iter->Valid());
}
TEST(BlockTest,Basic){
    BlockBuilder block_builder;
    Comparator cmp;
    block_builder.Add("1234","aa");
    block_builder.Add("2345","bb");
    std::string raw = block_builder.Finish();
    Block block(raw);
    auto iter = block.NewIterator(&cmp);
    iter->Seek("1234");
    ASSERT_TRUE(iter->Valid());
    ASSERT_EQ(iter->value(),"aa");
    iter->Seek("2345");
    ASSERT_TRUE(iter->Valid());
    ASSERT_EQ(iter->value(),"bb");
}