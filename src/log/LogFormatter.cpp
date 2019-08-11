#include <map>
#include <functional>
#include "LogFormatter.h"
#include "MessageFormatItem.h"
#include "LevelFormatItem.h"
#include "FileFormatItem.h"
#include "LineFormatItem.h"
#include "ThreadFormatItem.h"
#include "ThreadNameFormatItem.h"
#include "DateTimeFormatItem.h"
#include "StringFormatItem.h"
#include "TabFormatItem.h"

namespace youren{

LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern)
{
    init();
}

std::string LogFormatter::format(LogLevel::Level level, LogEvent::ptr event)
{
    std::stringstream ss;
    for(auto& i : m_items)
    {
        i->format(ss, level, event);
    }
    return ss.str();
}

std::ostream& LogFormatter::format(std::ostream& os,LogLevel::Level level,LogEvent::ptr event)
{
    for(auto& i:m_items)
    {
        i->format(os,level,event);
    }
    return os;
}

void LogFormatter::init()
{
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); i++)
    {
        if(m_pattern[i] != '%')
        {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        //处理 %% 这种情况
        if((i+1) < m_pattern.size())
        {
            if(m_pattern[i+1] == '%')
            {
                nstr.append(1, '%');
                continue;
            }
        }
        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                               && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            //处理%mdx 优先第一个
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }
    if(!nstr.empty())
    {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt){return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(d, DateTimeFormatItem),
        XX(f, FileFormatItem),
        XX(l, LineFormatItem),
        XX(t, ThreadFormatItem),
        XX(N, ThreadNameFormatItem),
        XX(T, TabFormatItem),
#undef XX
    };
    for(auto& i:vec)
    {
        if(std::get<2>(i) == 0)
        {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else
        {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end())
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %"+std::get<0>(i)+">>")));
                m_error =true;
            }
            else
            {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
    }
}


}
