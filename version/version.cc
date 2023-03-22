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

void Version::DeleteFile(int level,FileMetaData* meta){
    for (size_t i=0;i<files_[level].size();i++){
        FileMetaData* f = files_[i];
        if (meta->number = f->number){
            files[level].erase(files[level].begin+i);
            break;
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
    // auto iter = lower_bound(files_[level].begin(),files_[level].end(),key.key_,[&](const FileMetaData* a,const std::string b){
    //     return a->largest.key_<b;
    // });
    // return iter-files_[level].begin();
    return FindFile(files_[level],key.key_);
}
int Version::FindFile(const std::vector<FileMetaData*>& level_files,std::string target){
    auto iter = lower_bound(level_files.begin(),level_files.end(),target,[&](const FileMetaData* a,const std::string b){
        return a->largest<b;
    });
    return iter-files_[level].begin();;
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

Iterator* Version::MakeInputIterator(Compaction* c){
    // const int space = (c->level_ == 0 ? c->inputs_[0].size()+1:2);
    vector<Iterator*> list(space);
    // int num = 0
    for (size_t i=0;i<c->inputs_[0].size();i++){
        list.push_back(SSTable::Open(TableFileName(dbname_,c->inputs_[0][i]->number)).NewIterator());
    }
    for (size_t i=0;i<c->inputs_[1].size();i++){
        list.push_back(SSTable::Open(TableFileName(dbname_,c->inputs_[1][i]->number)).NewIterator());
    }
    // for (int which=0;which<2;which++){
    //     if (!c->inputs_[which].empty()){
    //         if (c->level_ + which ==0){
    //             const std::vector<FileMetaData*>& files = c->inputs_[which];
    //             for (size_t i=0;i<files.size();i++){
    //                 list[num++] = SSTable::Open(TableFileName(dbname_,files[i]->number)).NewIterator();
    //             }
    //         }else{
    //             list[num++] = NewTwoLevelIterator(
    //                 new Version::LevelFileNumIterator(c->inputs_[which]),
    //             )
    //         }
    //     }
    // }
    Iterator* result = NewMergingIterator(list);
    return result;
}

bool Version::DoCompactionWork(){
    Compaction* c = PickCompaction();
    if (c==nullptr) return false;
    if (c->IsTrivalMove()){
        for (FileMetaData* f:c->inputs_[0]){
            DeleteFile(c->level,f);
            AddFile(c->level+1,f);
        }
        return true;
    }

    Iterator* iter = MakeInputIterator(c);
    InternalKeyEntry *current=nullptr;
    vector<FileMetaData*> list;
    for (iter->SeekToFirst();iter->Valid();iter->Next()){
        FileMetaData* meta = new FileMetaData();
        meta->allowed_seeks = 1<<30;
        meta->number = v.NextFileNumber();
        SSTableBuilder builder = SSTableBuilder(TableFileName(dbname_,meta.number));
        meta->smallest = InternalKeyEntry(iter->key(),iter->value());
        for (;iter->Valid();iter->Next()){
            InternalKeyEntry p = InternalKeyEntry(iter->key(),iter->value());
            if (current!=nullptr){
                if (current->user_key() == p.user_key()){
                    continue;
                }
            }
            current = p;
            meta->largest = p;
            builder.Add(p);
            if (builder.FileSize()>MaxFileSize){
                break;
            }
        }
        builder.Finish();
        meta->file_size = uint64_t(builder.FileSize());
        list.push_back(meta);
    }
    for (int i=0;i<c->inputs_[0].size();i++){
        v.DeleteFile(c->level,c->inputs_[0][i]);
    }
    for (int i=0;i<c->inputs_[1].size();i++){
        v.DeleteFile(c->level+1,c->inputs_[1][i]);
    }
    for (int i=0;i<list.size();i++){
        v.AddFile(c->level+1,list[i]);
    }
    return true;
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

void Version::GetRange(const std::vector<FileMetaData*>& inputs,InternalKeyEntry& smallest,InternalKeyEntry& largest){
    for (size_t i=0;i<inputs.size();i++){
        FileMetaData* f = input[i];
        if (i==0){
            smallest = f->smallest;
            largest = f->largest;
        }else{
            if (f->smallest<smallest) {
                smallest = f->smallest;
            }
            if (f->largest>largest){
                largest = f->largest;
            }
        }
    }
}

void Version::GetRange2(const std::vector<FileMetaData*>& inputs1,
                        const std::vector<FileMetaData*>& inputs2,
                        InternalKeyEntry& smallest,InternalKeyEntry& largest){
    std::vector<FileMetaData*> all = inputs1;
    all.insert(all.end(),inputs2.begin(),inputs2.end());
    GetRange(all,smallest,largest);
                        }


Compaction* Version::PickCompaction(){
    Compaction* c;
    int level;
    const bool size_compaction = (current->compaction_score_>=1);
    if (size_compaction){
        level = current->compaction_level_;
        c = new Compaction();
        for (size_t i=0;i<files_[level].size();i++){
            FileMetaData* f = files[level][i];
            if (compact_pointer_[level].empty()||f->largest.key_>compact_pointer_[level]){
                c->inputs_[0].push_back(f);
                break;
            }
        }
        if (c->inputs_[0].empty()){
            c->inputs_[0].push_back(files_[level][0]);
        }
    }else{
        return nullptr;
    }
    InternalKeyEntry smallest,largest;
    if (level == 0){
        GetRange(c->inputs_[0],smallest,largest);
        GetOverlappingInputs(0,smallest,largest,c->inputs_[0]);
    }
    // set inputs_[1]
    AddBoundaryInputs(files_[level],c->inputs_[0]);
    GetRange(c->inputs_[0],smallest,largest);

    GetOverlappingInputs(level+1,smallest,largest,c->inputs_[1]);
    AddBoundaryInputs(files_[level+1],c->inputs_[1]);
    // set compact pointer
    compact_pointer_[level] = largest.key_();
    return c;
}
void Version::GetOverlappingInputs(int level,InternalKeyEntry& begin,InternalKeyEntry& end,std::vector<FileMetaData*>& inputs){
    inputs.clear();
    std::string user_begin,user_end;
    user_begin = begin.user_key();
    user_end = end.user_key();
    for (size_t i=0;i<files_[level][i].size();i++){
        FileMetaData* f = files_[level][i++];
        const std::string file_start = f->smallest.user_key();
        const std::striing file_limit = f->largest.user_key();
        if (file_limit<user_begin||file_start>user_end){
            continue;
        }else{
            inputs.push_back(f);
        }
    }
}
bool FindLargestKey(const std::vector<FileMetaData*>& files,InternalKey& largest_key){
    if (files.empty()) return false;

    largest_key = files[0]->largest;
    for (size_t i=1;i<files.size();i++){
        FileMetaData* f = files[i];
        if (f->largest>largest_key){
            largest_key = f->largest;
        }
    }
    return true;
}


FileMetaData* FindSmallestBoundaryFile(const std::vector<FileMetaData*>& level_files,const InternalKeyEntry& largest_key){
    FileMetaData* smallest_boundary_file = nullptr;
    for (size_t i=0;i<level_files.size();i++){
        FileMetaData* f = level_files[i];
        if (f->smallest>largest&&f->small_key.user_key()==largest_key.user_key()){
            if (smallest_boundary_file==nullptr||f->smallest<smallest_boundary_file->smallest){
                smallest_boundary_file = f;
            }
        }
    }
    return smallest_boundary_file;

}
void Version::AddBoundaryInputs(const std::vector<FileMetaData*>& level_files,std::vector<FileMetaData*>& compaction_files){
    InternalKeyEntry largest_key;
    if (!FindLargestKey(compaction_files,largest_key)){
        return;
    }
    bool continue_searching = true;
    while (continue_searching){
        FileMetaData* smallest_boundary_file = FindSmallestBoundaryFile(level_files,largest_key);
        if (smallest_boundary_file!=nullptr){
            compaction_files.push_back(smallest_boundary_file);
            largest_key = smallest_boundary_file->largest;
        }else{
            continue_searching = false;
        }
    }

}

Compaction* Version::PickCompactionLevel(){
    int best_level = -1;
    double best_score = -1;
    for (int level = 0;level<kNumLevels-1;k++){
        double score;
        if (level == 0 ){
            score = v->files[level].size()/static_cast<double>(kL0_CompactionTrigger);
        }else{
            const uint64_t level_bytes = TotalFileSize(v->files_[level]);
            score = static_cast<double>(level_bytes)/MaxFileSizeForLevel(level);
        }
    }
    if (score>best_score){
        best_level = level;
        best_score = score;
    }
    v->compaction_level_ = best_level;
    v->compaction_score_ = best_score;
}


uint64_t Version::TotalFileSize(std::vector<FileMetaData*> files){
    uint64_t totalfilesbytes = 0;
    for (auto file: files){
        totalfilesbytes+=file->file_size;
    }
    return totalfilesbytes;
}

static double MaxBytesForLevel(int level){
  double result = 10. * 1048576.0;
  while (level > 1) {
    result *= 10;
    level--;
  }
  return result;
}

bool Compaction::IsTrivialMove(){
    return (inputs_[0].size()==1&&inputs_[1].size()==0);
}