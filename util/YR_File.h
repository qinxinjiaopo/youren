#pragma once
#include "YR_Exception.h"
#include <string>
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
namespace youren {
struct YR_File_Exception : public YR_Exception
{
    YR_File_Exception(const string &buffer) : YR_Exception(buffer){};
    YR_File_Exception(const string &buffer, int err) : YR_Exception(buffer, err){};
    ~YR_File_Exception() throw(){};
};

class YR_File
{
public:
    YR_File() {}
    ~YR_File() {}
    static bool isFileExistEx(const string &sFullFileName, mode_t iFileType = S_IFREG);
    static bool isFileExist(const string &sFullFileName, mode_t iFileType = S_IFREG);
    static string simplifyDirectory(const string& path);
    static bool makeDir(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);
    static bool makeDirRecursive(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);
    static string extractFilePath(const string &sFullFileName);

private:

};
}
