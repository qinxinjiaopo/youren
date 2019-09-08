#include "YR_File.h"

namespace youren {


bool YR_File::isFileExistEx(const string &sFullFileName, mode_t iFileType /*= S_IFREG*/)
{
    struct stat f_stat;

    if (stat(sFullFileName.c_str(), &f_stat) == -1)
    {
        return false;
    }

    if (!(f_stat.st_mode & iFileType))
    {
        return false;
    }

    return true;
}
bool YR_File::isFileExist(const string &sFullFileName, mode_t iFileType /*= S_IFREG*/)
{
    struct stat f_stat;

    if (lstat(sFullFileName.c_str(), &f_stat) == -1)
    {
        return false;
    }

    if (!(f_stat.st_mode & iFileType))
    {
        return false;
    }

    return true;
}
string YR_File::simplifyDirectory(const string& path)
{
    string result = path;

    string::size_type pos;

    pos = 0;
    while((pos = result.find("//", pos)) != string::npos)
    {
        result.erase(pos, 1);
    }

    pos = 0;
    while((pos = result.find("/./", pos)) != string::npos)
    {
        result.erase(pos, 2);
    }

    while(result.substr(0, 4) == "/../")
    {
        result.erase(0, 3);
    }

    if(result == "/.")
    {
        return result.substr(0, result.size() - 1);
    }

    if(result.size() >= 2 && result.substr(result.size() - 2, 2) == "/.")
    {
        result.erase(result.size() - 2, 2);
    }

    if(result == "/")
    {
        return result;
    }

    if(result.size() >= 1 && result[result.size() - 1] == '/')
    {
        result.erase(result.size() - 1);
    }

    if(result == "/..")
    {
        result = "/";
    }

    return result;
}
bool YR_File::makeDir(const string &sDirectoryPath, mode_t iFlag /*= S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH*/)
{
    int iRetCode = mkdir(sDirectoryPath.c_str(), iFlag);
    if(iRetCode < 0 && errno == EEXIST)
    {
        return isFileExistEx(sDirectoryPath, S_IFDIR);
    }

    return iRetCode == 0;
}
bool YR_File::makeDirRecursive(const string &sDirectoryPath, mode_t iFlag /*= S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH*/)
{
    string simple = simplifyDirectory(sDirectoryPath);

    string::size_type pos = 0;
    for(; pos != string::npos; )
    {
        pos = simple.find("/", pos + 1);
        string s;
        if(pos == string::npos)
        {
            s = simple.substr(0, simple.size());
            return makeDir(s.c_str(), iFlag);
        }
        else
        {
            s = simple.substr(0, pos);
            if(!makeDir(s.c_str(), iFlag)) return false;
        }
    }
    return true;
}

string YR_File::extractFilePath(const string &sFullFileName)
{
    if(sFullFileName.length() <= 0)
    {
        return "./";
    }
    string::size_type pos = 0;
    for(pos = sFullFileName.length(); pos != 0 ; --pos)
    {
        if(sFullFileName[pos-1] == '/')
        {
            return sFullFileName.substr(0, pos);
        }
    }
    return "./";
}

}
