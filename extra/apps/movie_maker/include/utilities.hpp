#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <string>
#include <vector>


class Utilities
{
public:
static void GetFilesInFolder(const std::string& dirName, const std::string &pattern, 
                std::vector<std::string> &filesList);
};

#endif // __UTILITIES_HPP__
