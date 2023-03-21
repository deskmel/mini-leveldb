#include "../include/version/version.h"
#include "../include/utils/filename.h"
#include "../include/sstable/sstable.h"


Version* Version::Load(std::string dbname,uint64_t seqNum){
    std::string  manifest_file_name = DescriptorFileName(dbname,seqNum);
    Version* v = New(dbname);
    v->DecodeFrom(manifest_file_name);
    return v;
}

Version* Version::New(std::string dbname){
    Version* v = new Version();
    v->dbname_ = dbname;
    v->seqNum_ = 1;
    v->next_file_number_ = 1;
    return v;
}

Version* Version::Copy(){
    Version* v = Version::New(dbname_);
    v->next_file_number_ = next_file_number_;
    v->seqNum_ = seqNum_;
    for (int i=0;i<kNumLevels;i++){
        v->files_[i] = std::vector<FileMetaData*>(files_[i]);
    }
    
    return v;
}

int Version::NumLevelFiles(int level){
    std::lock_guard<std::mutex> l(mu_);
    return files_[0].size();
}

uint64_t Version::NextFileNumber(){
    std::lock_guard<std::mutex> l(mu_);
    return next_file_number_++;
}

uint64_t Version::LastSequence(){
    std::lock_guard<std::mutex> l(mu_);
    return seqNum_++;
}

void Version::DecodeFrom(std::string filename){
    std::ifstream file(filename.c_str());
    char buf[100];
    std::string scratch;
    file.read(buf,8);
    scratch.assign(buf,8);
    seqNum_ = DecodeFixed64(scratch);
    file.read(buf,8);
    scratch.assign(buf,8);
    next_file_number_ = DecodeFixed64(scratch);
    for (int i=0;i<kNumLevels;i++){
        file.read(buf,4);
        scratch.assign(buf,4);
        int file_num = DecodeFixed32(scratch);
        for (int i=0;i<file_num;i++){
            FileMetaData *file_meta = new FileMetaData();
            file_meta->DecodeFrom(&file);
            files_[i].push_back(file_meta);
        }
    }
}

void Version::EncodeTo(std::ofstream* file){
    std::string scratch;
    PutFixed64(scratch,seqNum_);
    PutFixed64(scratch,next_file_number_);
    for (int i=0;i<kNumLevels;i++){
        PutFixed32(scratch,files_[i].size());
        for (auto file:files_[i]){
            file->EncodeTo(scratch);
        }
    }
    file->write(scratch.data(),scratch.size());
}

void Version::AddFile(FileMetaData* meta,int level){
    if (level ==0 ){
        files_[level].push_back(meta);
    }else{
        int index = FindFile(level,meta->smallest);
        if (index == files_[level].size()){
            files_[level].push_back(meta);
        }else{
            files_[level].insert(files_[level].begin()+index,meta);
        }
    }
}

uint64_t  Version::Save(){
    uint64_t n = next_file_number_ ++;
    std::string filename= DescriptorFileName(dbname_,n);
    std::ofstream file(filename,std::ios::trunc);
    EncodeTo(&file);
    file.close();
    return n;
}

int Version::FindFile(int level,const InternalKeyEntry& key){
    auto iter = lower_bound(files_[level].begin(),files_[level].end(),key.key_,[&](const FileMetaData* a,const std::string b){
        return a->largest.key_<b;
    });
    return iter-files_[level].begin();
}

bool Version::Get(const LookupKey& key,std::string& value){
    for (int level=0;level<kNumLevels;level++){
        std::vector<FileMetaData*> find_files;
        if (level==0){
            // level zero
            for (int file_index = 0;file_index<files_[level].size();file_index++){
                FileMetaData* file_meta = files_[level][file_index];
                if (key.internal_key()>=file_meta->smallest.key_&&key.internal_key()<=file_meta->largest.key_){
                    find_files.push_back(file_meta);
                }
            }
        }else{
            auto iter = lower_bound(files_[level].begin(),files_[level].end(),key.internal_key(),[&](const FileMetaData* a,const std::string b){
                return a->largest.key_<b;
            });
            if (iter==files_[level].end()||(*iter)->largest.key_< key.internal_key()){
                continue;
            }else{
                find_files.push_back(*iter);
            }
        }
        for (auto file_meta:find_files){
            SSTable* table = SSTable::Open(TableFileName(dbname_,file_meta->number));
            bool success = table->InternalGet(key,value);
            if (success) return true;
        }

    }
    return false;
}


void Version::WriteLevel0Table(MemTable* imm){
    FileMetaData *meta=new FileMetaData();
    meta->allowed_seeks = 1<<30;
    meta->number = NextFileNumber();
    SSTableBuilder builder(TableFileName(dbname_,meta->number));
    auto iter = imm->NewIterator();
    iter->SeekToFirst();
    meta->smallest = InternalKeyEntry(iter->key(),iter->value());
    while (iter->Valid()){
        meta->largest = InternalKeyEntry(iter->key(),iter->value());
        builder.Add(iter->key(),iter->value());
        iter->Next();
    }
    builder.Finish();
    meta->file_size = builder.FileSize();
    AddFile(meta,0);
}

void FileMetaData::EncodeTo(std::string& content){
    PutFixed32(content,allowed_seeks);
    PutFixed32(content,number);
    PutFixed32(content,file_size);
    smallest.EncodeTo(content);
    largest.EncodeTo(content);
}

void FileMetaData::DecodeFrom(std::ifstream *file){
    int start = 0;
    char buf[12];
    file->read(buf,12);
    std::string content;
    content.assign(buf,12);
    allowed_seeks = DecodeFixed32(content);
    number = DecodeFixed32(content.substr(4));
    file_size = DecodeFixed32(content.substr(8));
    // content = content.substr(12);
    smallest.DecodeFrom(file);
    largest.DecodeFrom(file);
}





