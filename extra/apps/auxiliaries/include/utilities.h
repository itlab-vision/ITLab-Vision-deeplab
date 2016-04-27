#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <string>
#include <sstream>
#include <iomanip>

void getImageNameByIdx(const int idx, const std::string &ext,
    std::string &name, const int len = 6);

#endif
