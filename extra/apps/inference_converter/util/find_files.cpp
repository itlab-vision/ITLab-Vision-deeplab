#include "find_files.hpp"

#include <dirent.h>
#include <fnmatch.h>
#include <stdexcept>

void listDirectory(const std::string& path, const std::string& pattern, 
    bool stripExtension, std::vector<std::string>& fileNames) 
{
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        throw std::runtime_error("Error opening dir: '" + path + "'.");
    }

    dirent* entry = readdir(dir);
    if (entry == NULL) {
        closedir(dir);
        throw std::runtime_error("Directory : '" + path + "' is empty!");
    }

    while (entry != NULL) {
        DIR* testEntry = opendir(entry->d_name);
        
        if (testEntry == NULL) { // if not a directory
            if (fnmatch(pattern.c_str(), entry->d_name, 0) == 0) {
                if (stripExtension == true) {
                    std::string tmp(entry->d_name);                    
                    fileNames.emplace_back(tmp.substr(0, tmp.rfind(".")));
                } else {
                    fileNames.emplace_back(entry->d_name);
                }
            }
        } else {
            closedir(testEntry);
        }
    
        entry = readdir(dir);
    }

    closedir(dir);
}