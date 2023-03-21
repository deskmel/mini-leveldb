#include <unistd.h>
#include "../include/db/db.h"
#include "../include/memtable/memtable.h"
#include "../include/utils/filename.h"
#include "../include/utils/code.h"
#include "../include/utils/status.h"
DB::DB(){
    
}

DB* DB::Open(std::string dbname){
    auto db = new DB();
    db->name_ = dbname;
    db->mem_ = new MemTable(new InternalKeyComparator());
    db->imm_ = nullptr;
    bgCompactionScheduled_ = false;
    int seqNum = ReadCurrentFile();
    if (seqNum > 0){
        db->current_ = Version::Load(dbname,seqNum);
    }else{
        db->current_ = Version::New(dbname);
    }
    return db;
}

bool DB::Get(std::string key,std::string& value){
    std::lock_guard<std::mutex> lg(mu_);
    SequenceNumber seq = current_->LastSequence();
    LookupKey l(key,seq);
    std::string status;
    bool find = false;
    {
        mu_.unlock();
        find = mem_->Get(l,value,status);
        if (!find){
            find = imm_->Get(l,value,status);
        }
        if (!find){
            (!current_->Get(l,value));
        }
        mu_.lock();
    }
    return find;
}

void DB::Put(std::string& key,std::string& value){
    uint64_t seq = MakeRoomForWrite();
    mem_->Add(seq,kTypeValue,key,value);
    return;
}


void DB::Delete(std::string& key){
    uint64_t seq = MakeRoomForWrite();
    mem_->Add(seq,kTypeDeletion,key,"");
    return;
}

uint64_t DB::MakeRoomForWrite(){
    std::unique_lock<std::mutex> l(mu_);
    uint64_t seq = current_->LastSequence();
    bool allow_delay = true;
    while (true){
        if (allow_delay && current_->NumLevelFiles(0)>kL0_SlowdownWritesTrigger){
            mu_.unlock();
            sleep(1000);
            allow_delay = false;
            mu_.lock();
        }else if (mem_->ApproximateMemoryUsage() <= write_buffer_size){
            break;
        }else if (imm_ != nullptr){
            background_work_finished_signal_.wait(l);
        }else if (current_->NumLevelFiles(0) >= kL0_StopWritesTrigger){
            background_work_finished_signal_.wait(l);
        } else{
           imm_ = mem_;
           mem_ = new MemTable(new Comparator());
           MaybeScheduleCompaction();
        }
    }
    return seq;
}

void DB::MaybeScheduleCompaction(){
    if (background_compaction_scheduled_){
        return;
    } else{
        // BGWork();
        std::lock_guard<std::mutex> l(background_work_mutex_);
        if (background_work_count_==0){
            background_work_cv_.notify_one();
        }
        background_work_count_++;
    }
}
void DB::BGWork(){
    while (true){
        {
            std::unique_lock<std::mutex> l(background_work_mutex_);
            while (background_work_count_==0){
                background_work_cv_.wait(l);
            }
            background_work_count_-=1;
            // background_work_mutex_.unlock();
        }
        BackgroundCall();
    }
}

void DB::BackgroundCall(){
    std::lock_guard<std::mutex> l(mu_);
    BackgroundCompaction();
    background_compaction_scheduled_ = false;
    background_work_finished_signal_.notify_all();
}


void DB::BackgroundCompaction(){
	MemTable* imm = imm_;
	Version* version = current_->Copy();
	mu_.unlock();
    if (imm != nullptr){
        // minor compaction
        version->WriteLevel0Table(imm);
        return;
    }
    // major compaction
    uint64_t descriptorNumber = version->Save();
	SetCurrentFile(descriptorNumber);
	mu_.lock();
	imm_ = nullptr;
	current_ = version;
}


void DB::SetCurrentFile(uint64_t seq){
    std::ofstream file(TempFileName(name_,seq));
    std::string seq_buf;
    PutFixed64(seq_buf,seq);
    file.write(seq_buf.data(),8);
    file.close();
    std::rename(TempFileName(name_,seq).c_str(),CurrentFileName(name_).c_str());
}

uint64_t DB::ReadCurrentFile(){
    std::ifstream file(CurrentFileName(name_));
    if (!file){
        return 0;
    }
    char buf[8];
    file.read(buf,8);
    std::string seq_buf;
    seq_buf.assign(buf,8);
    uint64_t seqnum = DecodeFixed64(seq_buf);
    file.close();
    return seqnum;
}