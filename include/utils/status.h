#ifndef _STATUS_H_
#define _STATUS_H_
#include <string>

static const std::string OK_ = "OK";
static const std::string NotFound_ = "NotFound";
static const std::string Corruption_= "Corruption";


class Status{
public:
    Status(const std::string& state):state_(state){}
    bool ok() const {return (state_ == OK_);}
    bool IsNotFound() const {return (state_==NotFound_);}
    bool IsCorruption() const {return (state_==Corruption_);}
    static Status OK(){return Status(OK_);}
    static Status NotFound(){return Status(NotFound_);}
    static Status Corruption(){return Status(Corruption_);}
    
private:
    std::string state_;
};
#endif