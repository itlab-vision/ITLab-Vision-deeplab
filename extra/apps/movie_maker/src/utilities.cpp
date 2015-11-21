#include "utilities.hpp"

#ifdef WIN32
#include <windows.h>
#elif UNIX
// FIX: need to check
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#endif

void Utilities::GetFilesInFolder(const std::string& dirName,
        std::vector<std::string> &filesList)
{
#ifdef WIN32
    WIN32_FIND_DATAA fileData;
    std::string mask = "/*.jpg";
    HANDLE handle = FindFirstFileA((dirName + mask).c_str(), &fileData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        throw ("There are no files " + mask + " in " + dirName + ".");
    }
    do 
    {
        const std::string file_name = fileData.cFileName;
        const std::string full_file_name = dirName + "/" + file_name;
        filesList.push_back(full_file_name);
    } 
    while (FindNextFileA(handle, &fileData));
    FindClose(handle);
#elif UNIX
    // FIX: need to check
    DIR *dp;
    struct dirent *dirp;
    if ((dp  = opendir(dirName.c_str())) == NULL)
    {
        std::cout << "Error(" << errno << ") opening " << dir << endl;
        return 0;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 1;
#endif
}