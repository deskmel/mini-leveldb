个人的leveldb简化实现，实现了leveldb的主要组件
主要参考如下项目 
1. leveldb 源仓库 https://github.com/google/leveldb

目前实现了：
1. 内存table模块：  skiplist,memtable
2. 磁盘数据结构：   SSTABLE,BLOCK,SSTABLE_BUILDER,BLOCK_BUILDER
3. 单一版本控制:    version
4. 后台Compact:    minor Compact 以及 Major Compact
5. 基于锁的同步读写 

没有实现，且有可能将来会实现的：
1. 多版本控制
2. WriteBatch
3. TableCache
4. 数据压缩
5. Log 以及 Snapshot

