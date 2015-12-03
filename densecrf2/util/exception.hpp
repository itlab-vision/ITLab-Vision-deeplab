#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>        
#include <iostream>
#include <string>


class exception : public std::exception {
public:
    exception(const std::string& message) :
        message(message)
    {}

    exception(std::string&& message) :
        message(std::move(message))
    {}

    virtual ~exception() throw() {}

    virtual const char* what() const throw() {
        return message.c_str();
    }
private:
    std::string message;
};

#endif // EXCEPTION_H