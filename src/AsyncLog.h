//
// Created by Lijingxin on 2023/3/14.
//

#ifndef SOCKETSERVER_ASYNCLOG_H
#define SOCKETSERVER_ASYNCLOG_H

#include <stdio.h>
#include <string>
#include <list>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

enum LOG_LEVEL {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,        //输出日志后退出
    LOG_LEVEL_CRITICAL      //关键日志，忽略级别输出
};

class AsyncLog {

public:
    //psz是一个变量名的前缀，表示这个变量是一个指向以空字符结尾的字符串的指针。这是一种常见的Hungarian notation，用来标识变量的类型和用途。
    static bool
    init(const char *pszLogFileName = nullptr, bool bTruncateLongLine = false, int nRollSize = 10 * 1024 * 1024);
    static void uninit();

    static void setLevel(LOG_LEVEL level);
    static bool isRunning();

    //不输出线程ID号和所在函数签名、行号
    static bool output(LOG_LEVEL nLevel, const char* pszFmt, ...);
    //输出线程ID号和所在函数签名、行号
    static bool output(LOG_LEVEL nLevel, const char* pszFileName, int nLineNo, const char* pszFmt, ...);
    //输出二进制日志
//    static bool outputBinary(unsigned char* buffer, size_t size);

private:
    AsyncLog() = delete;
    ~AsyncLog() = delete;

    AsyncLog(const AsyncLog& rhs) = delete;
    AsyncLog& operator=(const AsyncLog& rhs) = delete;

    static void makeLinePrefix(LOG_LEVEL nLevel, std::string& strPrefix);
    static void getTime(char* pszTime, int nTimeStrLength);
    static bool createNewFile(const char* pszLogFileName);
    static bool writeToFile(const std::string& data);
    //让程序主动崩溃
    static void crash();

//    static const char* ullto4Str(int n);
//    static char* FormLog(int& index, char* szbuf, size_t sizeBuf, unsigned char* buffer, size_t size);
    static void writeThreadProc();

private:
    static bool m_bToFile;                                      //输出文件/控制台
    static FILE* m_hLogFile;                                    //文件指针
    static std::string m_strFileName;                           //文件名
    static std::string m_strFileNamePid;                        //文件名中的进程id
    static bool m_bTruncateLongLog;                             //长日志截断标志
    static LOG_LEVEL m_nCurrentLevel;                           //当前日志等级
    static int m_iFileRollSize;                                 //单个日志文件最大字节数
    static int m_iCurrentWriteSize;                             //当前写入字节数
    static bool m_bExit;                                        //退出标志
    static bool m_bRunning;                                     //运行标志
    static std::list<std::string> m_listLinesToWrite;           //待写入日志
    static std::shared_ptr<std::thread> m_spWriteThread;
    static std::mutex m_mutexWrite;
    static std::condition_variable m_cvWrite;

};


#endif //SOCKETSERVER_ASYNCLOG_H
