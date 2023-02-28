#ifndef _COMPARATOR_H_
#define _COMPARATOR_H_
#include <string>
class Comparator{
    public:
    Comparator(){}
    int compare(const std::string& a,const std::string& b)const{
        if (a>b) return 1;
        if (a==b) return 0;
        return -1;
    }
};
#endif