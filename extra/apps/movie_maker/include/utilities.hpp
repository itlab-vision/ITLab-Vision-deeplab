#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <string>
#include <vector>

class Utilities
{
public:
static void GetFilesInFolder(const std::string& dirName,
                const std::string &mask,
                std::vector<std::string> &filesList);
};

#endif
