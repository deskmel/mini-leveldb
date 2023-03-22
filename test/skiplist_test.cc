#include <string>

#include "../include/skiplist/skiplist.h"
#include "../include/skiplist/comparator.h"
#include "../include/utils/testutils.h"


TEST(SkipTest,Empty){
    Comparator cmp;
    SkipList list(&cmp);
    ASSERT_TRUE(!list.Contains("10"));

    SkipList::Iterator iter(&list);
    ASSERT_TRUE(!iter.Valid());
    iter.SeekToFirst();
    ASSERT_TRUE(!iter.Valid());
    iter.Seek("100");
    ASSERT_TRUE(!iter.Valid());
    iter.SeekToLast();
    ASSERT_TRUE(!iter.Valid());
}

TEST(SkipTest,InsertAndLookup){
    const int N = 2000;
    const int R = 5000;
    std::set<std::string> keys;
    Comparator cmp;
    SkipList list(&cmp);
    for (int i=0;i<N;i++){
        std::string key = std::to_string(rand()%R);
        if (keys.insert(key).second){
            list.Insert(key);
        }
    }

    for (int i = 0; i < R; i++) {
        std::string key = std::to_string(i);
        if (list.Contains(key)) {
        ASSERT_EQ(keys.count(key), 1);
        } else {
        ASSERT_EQ(keys.count(key), 0);
        }
    }
    // simple iterator tests

    {
        SkipList::Iterator iter(&list);
        ASSERT_TRUE(!iter.Valid());

        iter.Seek("0");
        ASSERT_TRUE(iter.Valid());
        ASSERT_EQ(*(keys.begin()), iter.key());

        iter.SeekToFirst();
        ASSERT_TRUE(iter.Valid());
        ASSERT_EQ(*(keys.begin()), iter.key());

        iter.SeekToLast();
        ASSERT_TRUE(iter.Valid());
        ASSERT_EQ(*(keys.rbegin()), iter.key());
    }

    // Forward iteration test
    for (int i = 0; i < R; i++) {
        std::string key = std::to_string(i);
        SkipList::Iterator iter(&list);
        iter.Seek(key);

        // Compare against model iterator
        std::set<std::string>::iterator model_iter = keys.lower_bound(key);
        for (int j = 0; j < 3; j++) {
        if (model_iter == keys.end()) {
            ASSERT_TRUE(!iter.Valid());
            break;
        } else {
            ASSERT_TRUE(iter.Valid());
            ASSERT_EQ(*model_iter, iter.key());
            ++model_iter;
            iter.Next();
        }
        }
    }

    // Backward iteration test
    {
        SkipList::Iterator iter(&list);
        iter.SeekToLast();

        // Compare against model iterator
        for (std::set<std::string>::reverse_iterator model_iter = keys.rbegin();
            model_iter != keys.rend(); ++model_iter) {
        ASSERT_TRUE(iter.Valid());
        ASSERT_EQ(*model_iter, iter.key());
        iter.Prev();
        }
        ASSERT_TRUE(!iter.Valid());
    }
}