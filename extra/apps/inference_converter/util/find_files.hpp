#ifndef FIND_FILES_H
#define FIND_FILES_H

#include <vector>
#include <string>

void listDirectory(const std::string& path, const std::string& pattern, bool stripExtension, std::vector<std::string>& fileNames);

#endif // FIND_FILES_H