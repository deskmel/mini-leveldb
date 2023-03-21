cmake_minimum_required(VERSION 3.2)
project(neverdb)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS}  -O2 -Wall -g -ggdb")


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


file(GLOB skiplist_sources  ./include/skiplist/*.h ./skiplist/*.cc)
file(GLOB memtable_sources ./include/memtable/*.h ./include/db/dbformat.h ./include/iterator/*.h ./iterator/*.cc ./memtable/*.cc ./db/dbformat.cc ./utils/code.cc)

file(GLOB block_sources  ./include/sstable/block_builder.h 
                    ./include/sstable/block_handle.h
                    ./include/sstable/block.h
                    ./sstable/block_builder.cc
                    ./sstable/block_handle.cc
                    ./sstable/block.cc)

file(GLOB table_sources ./include/sstable/sstable_builder.h
                        ./include/sstable/sstable.h
                        ./include/sstable/two_level_iterator.h
                        ./sstable/two_level_iterator.cc
                        ./sstable/sstable.cc
                        ./sstable/sstable_builder.cc)

file(GLOB version_sources ./include/version/version.h
                          ./version/version.cc)


file(GLOB db_sources  ./include/db/db.h
                      ./include/utils/config.h
                      ./include/utils/filename.h
                      ./db/db.cc
                      ./utils/filename.cc)                        

add_library(test_utils ./utils/testutils.cc)
target_link_libraries(test_utils gtest)





include(GoogleTest)



add_executable(skiplist_test ./test/skiplist_test.cc)
target_link_libraries(skiplist_test gtest_main test_utils ${skiplist_sources}  )

add_executable(memtable_test ./test/memtable_test.cc)
target_link_libraries(memtable_test gtest_main test_utils ${memtable_sources} ${skiplist_sources})

add_executable(block_test ./test/block_test.cc)
target_link_libraries(block_test gtest_main test_utils ${memtable_sources} ${skiplist_sources} ${block_sources})

add_executable(table_test ./test/table_test.cc)
target_link_libraries(table_test gtest_main test_utils ${table_sources} ${memtable_sources} ${skiplist_sources} ${block_sources})

add_executable(version_test ./test/version_test.cc)
target_link_libraries(version_test gtest_main test_utils ${table_sources} ${memtable_sources} ${skiplist_sources} ${block_sources} ${version_sources} ${db_sources})

add_executable(version_test1 ./test/version_test1.cc)
target_link_libraries(version_test1 gtest_main test_utils ${table_sources} ${memtable_sources} ${skiplist_sources} ${block_sources} ${version_sources} ${db_sources})


gtest_discover_tests(skiplist_test)
gtest_discover_tests(memtable_test)
gtest_discover_tests(block_test)
gtest_discover_tests(version_test)