#include "../include/utils/testutils.h"
#include "../include/skiplist/comparator.h"
#include "../include/memtable/memtable.h"

TEST(TableTest,Empty){
    Comparator* cmp = new InternalKeyComparator();
    MemTable table(cmp);
    Iterator* iter = table.NewIterator();
    ASSERT_TRUE(!iter->Valid());
    iter->SeekToFirst();
    ASSERT_TRUE(!iter->Valid());
    iter->Seek("100");
    ASSERT_TRUE(!iter->Valid());
    iter->SeekToLast();
    ASSERT_TRUE(!iter->Valid());
}

TEST(TableTest,InsertAndLookup){
    const int N = 2000;
    const int R = 5000;
    std::set<std::string> keys;
    Comparator *cmp = new InternalKeyComparator();
    MemTable table(cmp);
    Iterator *iter = table.NewIterator();
    for (int i=0;i<N;i++){
        std::string key = std::to_string(rand()%R);
        if (keys.insert(key).second){
            table.Add(0,kTypeValue,key,"");
        }
    }

    // for (int i = 0; i < R; i++) {
    //     std::string key = std::to_string(i);
    //     if (list.Contains(key)) {
    //     ASSERT_EQ(keys.count(key), 1);
    //     } else {
    //     ASSERT_EQ(keys.count(key), 0);
    //     }
    // }
    // simple iterator tests

    // {
    //     Iterator *iter = table.NewIterator();
    //     ASSERT_TRUE(!iter->Valid());

    //     iter->Seek("0");
    //     ASSERT_TRUE(iter->Valid());
    //     ASSERT_EQ(*(keys.begin()), iter->key());

    //     iter->SeekToFirst();
    //     ASSERT_TRUE(iter->Valid());
    //     ASSERT_EQ(*(keys.begin()), iter->key());

    //     iter->SeekToLast();
    //     ASSERT_TRUE(iter->Valid());
    //     ASSERT_EQ(*(keys.rbegin()), iter->key());
    // }

    // Forward iteration test
    for (int i = 0; i < R; i++) {
        std::string key = std::to_string(i);
        Iterator *iter = table.NewIterator();
        iter->Seek(key);

        // Compare against model iterator

        std::set<std::string>::iterator model_iter = keys.lower_bound(key);
        for (int j = 0; j < 3; j++) {
            if (model_iter == keys.end()) {
                ASSERT_TRUE(!iter->Valid());
                break;
            } else {
                ASSERT_TRUE(iter->Valid());
                ASSERT_EQ(*model_iter, iter->key().substr(0,iter->key().size()-8));
                // printf("%s %s %s\n",key.c_str(),(*model_iter).c_str(), iter->key().c_str());
                ++model_iter;
                iter->Next();
            }
        }
    }

    // Backward iteration test
    {
        Iterator *iter = table.NewIterator();
        iter->SeekToLast();

        // Compare against model iterator
        for (std::set<std::string>::reverse_iterator model_iter = keys.rbegin();
            model_iter != keys.rend(); ++model_iter) {
        ASSERT_TRUE(iter->Valid());
        ASSERT_EQ(*model_iter, iter->key().substr(0,iter->key().size()-8));
        iter->Prev();
        }
        ASSERT_TRUE(!iter->Valid());
    }

    // Forward iteration get test
    for (int i = 0; i < R; i++) {
        std::string key = std::to_string(i);
        LookupKey lookupkey(key,0);
        std::string value;
        std::string status;
        bool find = table.Get(lookupkey,value,status);

        // Compare against model iterator
        std::set<std::string>::iterator model_iter = keys.find(key);

        if (model_iter == keys.end()) {
            ASSERT_TRUE(!find);
            break;
        } else {
            printf("key %s\n",(*model_iter).c_str());
            ASSERT_TRUE(find);
            ASSERT_EQ(*model_iter, lookupkey.user_key());
            ++model_iter;
            iter->Next();
        }
        
    }
}