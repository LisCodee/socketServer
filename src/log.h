#ifndef __LOG_H__
#define __LOG_H__

#include<iostream>
#include<fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "map"

/**
 * 日志基类
 */
class LogBase
{
public:
    enum LOG_LEVEL{
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        FATAL = 4
    };

public:
    LogBase(LOG_LEVEL l, bool toFile, bool truncate, std::string outputFile):m_eLevel(l), m_bToFile(toFile), m_bTruncate(truncate)
    {
        if(toFile)
            m_fStream = new std::fstream(outputFile, std::ios::in | std::ios::app);
    };
    virtual ~LogBase()
    {
        delete m_fStream;
    };

    virtual void debug(std::string detail, std::string callInfo="", std::string threadInfo="") = 0;
    virtual void info(std::string detail, std::string callInfo="", std::string threadInfo="") = 0;
    virtual void warning(std::string detail, std::string callInfo="", std::string threadInfo="") = 0;
    virtual void error(std::string detail, std::string callInfo="", std::string threadInfo="") = 0;
    virtual void fatal(std::string detail, std::string callInfo="", std::string threadInfo="") = 0;
    void outLog(LOG_LEVEL l, std::string content);

protected:
    std::string getOutputInfo(LOG_LEVEL l, std::string detail, std::string threadInfo="", std::string callInfo="");

protected:
    LOG_LEVEL       m_eLevel;                   //输入的日志级别
    bool            m_bToFile;                  //写入文件还是控制台
    bool            m_bTruncate;                //是否截断长日志
    std::fstream*   m_fStream = nullptr;        //文件输出流
    std::string     m_logName;                  //日志文件名字
    int             m_iTruncateCount = 512;     //截断数
    int             m_iMaxFileSize = 10*1024;   //单个日志最大容量，10MB
public:
//    static std::mutex      m_mutexFStream;             //fstream锁
};

/**
 * 同步写日志类
 */
class SyncLogger: public LogBase
{
private:
    SyncLogger(LOG_LEVEL l = INFO, std::string outputFile = "", bool toFile = true, bool truncate = false):LogBase(l, toFile, truncate, outputFile){};
    ~SyncLogger(){};

    //禁止拷贝和复制构造函数
    SyncLogger(const SyncLogger& rhs) = delete;
    SyncLogger& operator=(const SyncLogger& rhs) = delete;

public:
    static SyncLogger* getSyncLogger(LOG_LEVEL l, std::string outputFile, bool toFile= true, bool truncate = false);
    void log(std::string detail, std::string callInfo="", std::string threadInfo="", LOG_LEVEL = INFO );
    void debug(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void info(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void warning(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void error(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void fatal(std::string detail, std::string callInfo="", std::string threadInfo="") override;
};

/**
 * 异步写日志类
 */
class AsyncLogger:public LogBase
{
public:
    struct LogItem{
        LOG_LEVEL       iLevel;
        std::string     iContent;
    };
private:
    AsyncLogger(LOG_LEVEL l = INFO, std::string outputFile = "", bool toFile = true, bool truncate = false):
    LogBase(INFO, toFile, truncate, outputFile){};
    ~AsyncLogger(){};

    AsyncLogger(const AsyncLogger&) = delete;
    AsyncLogger& operator=(const AsyncLogger&) = delete;

public:
    static AsyncLogger* getAsyncLogger(LOG_LEVEL l, std::string outputFile, bool toFile= true, bool truncate = false, int numThreads=2);
    void debug(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void info(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void warning(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void error(std::string detail, std::string callInfo="", std::string threadInfo="") override;
    void fatal(std::string detail, std::string callInfo="", std::string threadInfo="") override;

private:
    void addLogToBuffer(LOG_LEVEL, std::string);

public:
    static std::vector<LogItem>                             logBuffer;
    static std::mutex                                       mutex_logBuffer;
    static std::condition_variable                          mutex_cv;
    static std::vector<std::thread*>                        writeThreads;
};

#endif