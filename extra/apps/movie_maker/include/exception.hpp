#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include <exception>

class exception : public std::exception {
public:
    exception(const std::string& message) :
        message(message)
    {}

    exception(std::string&& message) :
        message(std::move(message))
    {}

    virtual ~exception() {}

    virtual const char* what() const {
        return message.c_str();
    }
private:
    std::string message;
};

#endif