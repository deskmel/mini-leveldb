 #include "../include/utils/code.h"
 
 void EncodeFixed32(std::string& buffer,uint32_t value){

  // Recent clang and gcc optimize this to a single mov / str instruction.
  buffer[0] = static_cast<uint8_t>(value);
  buffer[1]= static_cast<uint8_t>(value >> 8);
  buffer[2]= static_cast<uint8_t>(value >> 16);
  buffer[3]= static_cast<uint8_t>(value >> 24);
}

 uint32_t DecodeFixed32(const std::string& buffer) {

  // Recent clang and gcc optimize this to a single mov / ldr instruction.
  return (static_cast<uint32_t>(buffer[0])) |
         (static_cast<uint32_t>(buffer[1]) << 8) |
         (static_cast<uint32_t>(buffer[2]) << 16) |
         (static_cast<uint32_t>(buffer[3]) << 24);
}


 void EncodeFixed64(std::string& buffer,uint64_t value){

    buffer[0] = static_cast<uint8_t>(value);
    buffer[1] = static_cast<uint8_t>(value >> 8);
    buffer[2] = static_cast<uint8_t>(value >> 16);
    buffer[3] = static_cast<uint8_t>(value >> 24);
    buffer[4] = static_cast<uint8_t>(value >> 32);
    buffer[5] = static_cast<uint8_t>(value >> 40);
    buffer[6] = static_cast<uint8_t>(value >> 48);
    buffer[7] = static_cast<uint8_t>(value >> 56);

}

 uint64_t DecodeFixed64(const std::string& buffer){
  return (static_cast<uint64_t>(buffer[0])) |
         (static_cast<uint64_t>(buffer[1]) << 8) |
         (static_cast<uint64_t>(buffer[2]) << 16) |
         (static_cast<uint64_t>(buffer[3]) << 24) |
         (static_cast<uint64_t>(buffer[4]) << 32) |
         (static_cast<uint64_t>(buffer[5]) << 40) |
         (static_cast<uint64_t>(buffer[6]) << 48) |
         (static_cast<uint64_t>(buffer[7]) << 56);
}


void PutFixed32(std::string& dst, uint32_t value){
    std::string tmp;
    tmp.resize(4);
    EncodeFixed32(tmp,value);
    dst += tmp;
}
void PutFixed64(std::string& dst, uint64_t value){
    std::string tmp;
    tmp.resize(8);
    EncodeFixed64(tmp,value);
    dst += tmp;
}