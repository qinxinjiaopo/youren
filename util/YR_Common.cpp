#include "YR_Common.h"

#include <algorithm>
#include <sys/time.h>

namespace youren{
string YR_Common::trim(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
        return sStr;
    if(!bChar)		//当bChar为false，则去除s字符串
        //先去掉字符串左边的字符串，再去掉右边的字符串
        return trimright(trimleft(sStr, s, false), s, false);
    return trimright(trimleft(sStr, s, true), s, true);
}

string YR_Common::trimleft(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
        return sStr;
	//如果bChar为false，则去除s字符串
    if(!bChar)
    {
        if(sStr.length() < s.length())
            return sStr;
        if(sStr.compare(0, s.length(), s) == 0)
        {
            return sStr.substr(s.length());
        }
        return sStr;
    }
    /**
    * 去掉sStr左边的 字符串s中的字符
    */
    return sStr.substr(sStr.find_first_not_of(s));
}

string YR_Common::trimright(const string &sStr, const string &s, bool bChar)
{
    if(sStr.empty())
        return sStr;
    /**
    * 去掉sStr右边的字符串s
    */
    if(!bChar)
    {
        if(sStr.length() < s.length())
        {
            return sStr;
        }
        if(sStr.compare(sStr.length() - s.length(), s.length(), s) == 0)
        {
            return sStr.substr(0, sStr.length() - s.length());
        }

        return sStr;
    }
    /**
    * 去掉sStr右边的 字符串s中的字符
    */
    return sStr.substr(0, sStr.find_first_not_of(s));
}
//toupper和tolower原型：
//int toupper(int c);
//int tolower(int c);
string YR_Common::lower(const string &s)
{
    string sStr = s;
    for(size_t i=0;i<sStr.length();i++)
        sStr[i] = tolower(sStr[i]);
    return sStr;
}
string YR_Common::upper(const string &s)
{
    string sStr = s;
    for(size_t i=0;i<sStr.length();i++)
        sStr[i] = toupper(sStr[i]);
    return sStr;
}

bool YR_Common::isdigit(const string &sInput)
{
    return std::count_if(sInput.begin(),sInput.end(),[](char c){
        return ::isdigit(c);
    }) == static_cast<uint32_t>(sInput.length());
}
//strptime和strftime原型：
// #include <time.h>
//char *strptime(const char *s, const char *format, struct tm *tm);
//std::size_t strftime( char* str, std::size_t count, const char* format, const std::tm* time );
//struct tm {
//      int tm_sec;    /* Seconds (0-60) */
//      int tm_min;    /* Minutes (0-59) */
//      int tm_hour;   /* Hours (0-23) */
//      int tm_mday;   /* Day of the month (1-31) */
//      int tm_mon;    /* Month (0-11) */
//      int tm_year;   /* Year - 1900 */
//      int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
//      int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
//      int tm_isdst;  /* Daylight saving time */
//};

int YR_Common::strTotm(const string &sString, const string &sFormat, struct tm &stTm)
{
    char *p = strptime(sString.c_str(), sFormat.c_str(), &stTm);
    return (p != NULL) ? 0 : -1;
}
string YR_Common::tmTostr(const struct tm &stTm, const string &sFormat)
{
    char sTimeString[255] = "\0";
    strftime(sTimeString, sizeof(sTimeString), sFormat.c_str(), &stTm);
    return string(sTimeString);
}
string YR_Common::tmTostr(const time_t &t, const string &sFormat)
{
    struct tm tt;
    localtime_r(&t, &tt);
    return tmTostr(tt, sFormat);
}
string YR_Common::nowTostr(const string &sFormat)
{
    time_t t = time(NULL);
    return tmTostr(t, sFormat.c_str());
}
//原型：
// #include <sys/time.h>
//int gettimeofday(struct timeval *tv, struct timezone *tz);
//int settimeofday(const struct timeval *tv, const struct timezone *tz);
//struct timeval {
//	time_t      tv_sec;     /* seconds */
//	suseconds_t tv_usec;    /* microseconds */
//};

int64_t YR_Common::nowToms()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;
}

int64_t YR_Common::nowTous()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec * (int64_t)1000000 + tv.tv_usec;
}
}
