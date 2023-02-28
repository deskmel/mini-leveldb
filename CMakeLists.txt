cmake_minimum_required(VERSION 3.2)
project(neverdb)
file(GLOB sources CONFIGURE_DEPENDS ./include/skiplist/*.h ./skiplist/*.cc)



add_executable(skiplist_test ./test/skiplist_test.cc)
target_link_libraries(skiplist_test PUBLIC ${sources})

