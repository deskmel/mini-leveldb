#include "../include/utils/testutils.h"

std::string RandomString(int len,std::string* dst){
    dst->resize(len);
    for (int i=0;i<len;i++){
        (*dst)[i] = static_cast<char>(' '+rand()%95);
    }
    return (*dst);
}

std::string RandomKey(int len){
    static const char kTestChars[] = {'\0', '\1', 'a',    'b',    'c',
                                    'd',  'e',  '\xfd', '\xfe', '\xff'};
    std::string result;
    for (int i=0;i<len;i++){
        result += kTestChars[rand()%(sizeof(kTestChars))];
    }
    return result;
}