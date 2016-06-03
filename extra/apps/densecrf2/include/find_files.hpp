#ifndef FIND_FILES_HPP
#define FIND_FILES_HPP

#include <string>
#include <vector>

void listDirectory(const std::string& path, const std::string& pattern, 
    bool stripExtension, std::vector<std::string>& fileNames);

#endif // FIND_FILES_HPP