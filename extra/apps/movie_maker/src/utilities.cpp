#include "utilities.hpp"


#ifdef WIN32
    #include <windows.h>
#elif UNIX
    // FIX: need to check
    #include <sys/types.h>
    #include <dirent.h>
    #include <errno.h>
    #include <fnmatch.h>
#endif

#include "exception.hpp"


void Utilities::GetFilesInFolder(const std::string& dirName, const std::string& pattern, 
        std::vector<std::string> &filesList)
{
#ifdef WIN32
    WIN32_FIND_DATAA fileData;
    const std::string mask = "/" + pattern;
    HANDLE handle = FindFirstFileA((dirName + mask).c_str(), &fileData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        throw exception("There are no files matching pattern '" + pattern + "' in '" + dirName + "'.");
    }

    do 
    {
        const std::string file_name = fileData.cFileName;
        const std::string full_file_name = dirName + "/" + file_name;
        filesList.push_back(full_file_name);
    } 
    while (FindNextFileA(handle, &fileData) != 0);
    
    FindClose(handle);
#elif UNIX
    DIR* dir = opendir(dirName.c_str());
    if (dir == NULL) 
    {
        throw exception("Error opening dir: '" + dirName + "'. Error code " + std::to_string(errno) + ".");
    }

    dirent* entry = readdir(dir);
    if (entry == NULL) 
    {
        throw exception("Directory '" + dirName + "' is empty.");
    }

    while (entry != NULL) 
    {
        DIR* testEntry = opendir(entry->d_name);

        if (testEntry == NULL) 
        { // if not a directory
            if (fnmatch(pattern.c_str(), entry->d_name, 0) == 0)
            { //if name matches pattern
                filesList.emplace_back(entry->d_name);
            }
        } else 
        {
            closedir(testEntry);
        }

        entry = readdir(dir);
    }

    closedir(dir);
#endif
}