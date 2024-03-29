#include "../include/sstable/sstable_builder.h"

SSTableBuilder::SSTableBuilder(std::string filename)
    :   offset_(0),
        num_entries_(0),
        pending_index_entry_(false)
{
    file_ = new std::ofstream(filename.c_str(),std::ofstream::trunc|std::ios::binary);   
    data_block_builder_ = new BlockBuilder();
    index_block_builder_ = new BlockBuilder();
    pending_index_handle_ = new BlockHandle();
}

SSTableBuilder::~SSTableBuilder(){
    
    
    delete data_block_builder_;
    delete index_block_builder_;
    delete pending_index_handle_;
}

void SSTableBuilder::Add(const std::string& key,const std::string& value){
    if (pending_index_entry_){
        std::string handle_encoding;
        pending_index_handle_->EncodeTo(handle_encoding);
        index_block_builder_->Add(pending_index_key_,handle_encoding);
        pending_index_entry_ = false;
    }
    pending_index_key_.assign(key.data(),key.size());
    num_entries_++;
    data_block_builder_->Add(key,value);
    const size_t estimated_block_size = data_block_builder_->CurrentSizeEstimate();
    if (estimated_block_size >= SSTableBuilder::MAX_BLOCK_SIZE){
        Flush();
    }
}


void SSTableBuilder::Flush(){
    if (data_block_builder_->empty()){
        return;
    }
    WriteBlock(data_block_builder_,pending_index_handle_);
    pending_index_entry_ = true;

}

bool SSTableBuilder::Finish(){
    Flush();
    if (pending_index_entry_){
        std::string handle_encoding;
        pending_index_handle_->EncodeTo(handle_encoding);
        index_block_builder_->Add(pending_index_key_,handle_encoding);
        pending_index_entry_ = false;
    }
    BlockHandle index_block_handle;
    WriteBlock(index_block_builder_,&index_block_handle);
    Footer footer;
    footer.set_index_handle(index_block_handle);
    std::string footer_encoding;
    footer.EncodeTo(footer_encoding);
    // file_->seekp(0,std::ios::end);
    file_->write(footer_encoding.data(),footer_encoding.size());
    // printf("present p is %ld\n",file_->tellp());
    offset_ += footer_encoding.size();
    file_->flush();
    file_->close();
    return true;
}

void SSTableBuilder::Abandon(){
    // close_ = true;
}

uint64_t SSTableBuilder::NumEntries()const{
    return num_entries_;
}

uint64_t SSTableBuilder::FileSize() const {
    return offset_;
}

void SSTableBuilder::WriteBlock(BlockBuilder* block,BlockHandle* handle){
    std::string content = block->Finish();
    handle->set_offset(offset_);
    handle->set_size(content.size());
    // file_->seekp(0,std::ios::end);
    file_->write(content.data(),content.size());
    offset_ += content.size();
    block->Reset();   
}

