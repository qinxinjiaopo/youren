#pragma once
#include <string>

using namespace std;

namespace youren{

class YR_Common
{
public:
    //去掉头部及尾部的字符或者字符串
    //bool为true 去除s中的每个字符，为false去除s字符串
    static string trim(const string& sStr, const string& s=" \r\n\t", bool bChar=true);
    //去掉左边的字符或者字符串
    static string trimleft(const string &sStr, const string &s = " \r\n\t", bool bChar = true);
    //去掉右边的字符或者字符串
    static string trimright(const string &sStr, const string &s = " \r\n\t", bool bChar = true);
    //字符串转换成小写
    static string lower(const string &sString);
    //字符串转换成大写
    static string upper(const string &sString);
    //字符串是否都是数字
    static bool isdigit(const string &sInput);
    //字符串转换成时间结构
    static int strTotm(const string &sString, const string &sFormat, struct tm &stTm);
    //时间转换为字符串,默认格式为sFormat格式
    static string tmTostr(const struct tm &stTm, const string &sFormat = "%Y%m%d%H%M%S");
    static string tmTostr(const time_t &t, const string &sFormat = "%Y%m%d%H%M%S");
    //当前时间转换为字符串
    static string nowTostr(const string &sFormat = "%Y%m%d%H%M%S");
    //获取当前时间的毫秒数
    static int64_t nowToms();
    //获取当前时间的微秒数
    static int64_t nowTous();
    
    //字符串转换函数
    template<typename T>
    static T strto(const string &sStr);
    template<typename T>
    static T strto(const string &sStr, const string &sDefault);
    
};
}
