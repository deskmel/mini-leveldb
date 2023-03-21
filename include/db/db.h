#ifndef _DB_H_
#define _DB_H_
#include <string>
#include <mutex>
#include <condition_variable>
#include "../memtable/memtable.h"
#include "../version/version.h"
#include "./dbformat.h"

class DB {
    public:
    DB* Open(std::string DBName);
    void Close();
    bool Get(std::string key,std::string& value);
    void Put(std::string& key,std::string& value);
    void Delete(std::string& key);
    
    private:
    uint64_t ReadCurrentFile();
    void SetCurrentFile(uint64_t seq);
    void BackgroundCompaction();
    void BackgroundCall();
    void BGWork();
    void MaybeScheduleCompaction();
    uint64_t MakeRoomForWrite();
    DB();
    std::mutex mu_; 
    std::mutex background_work_mutex_;
    std::string name_;
    std::condition_variable cond_;
    std::condition_variable background_work_finished_signal_;
    std::condition_variable background_work_cv_;
    bool background_compaction_scheduled_;
    int background_work_count_;
    MemTable *mem_;
    MemTable *imm_;
    Version *current_;
    bool bgCompactionScheduled_;

};

#endif