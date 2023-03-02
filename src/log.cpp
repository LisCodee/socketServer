//
// Created by Lijingxin on 2023/3/1.
//
#include "log.h"
#include <iostream>
#include <mutex>
#include "ctime"
#include "utils.h"
#include "string"
#include "sstream"
//std::mutex LogBase::m_mutexFStream;
SyncLogger* SyncLogger::getSyncLogger(LOG_LEVEL l, std::string outputFile, bool toFile, bool truncate)
{
    static SyncLogger* syncLogger;
    std::string logFile = outputFile;
    if(syncLogger == nullptr)
    {
        if(toFile)
        {
            if (logFile.empty())
            {
                long long now = utils::getMicroseconds();
                std::stringstream fileName;
                fileName << now << ".log";
                logFile = fileName.str();
            }
        }
        syncLogger = new SyncLogger(l, logFile, toFile, truncate);
    }
    return syncLogger;
}

std::string LogBase::getOutputInfo(LOG_LEVEL l, std::string detail, std::string threadInfo, std::string callInfo)
{
    std::string timeInfo, levelInfo, res;
    switch (l) {
        case SyncLogger::DEBUG :
            levelInfo = "[DEBUG]";
            break;
        case SyncLogger::INFO :
            levelInfo = "[INFO]";
            break;
        case SyncLogger::WARNING :
            levelInfo = "[WARNING]";
            break;
        case SyncLogger::ERROR:
            levelInfo = "[ERROR]";
            break;
        case SyncLogger::FATAL:
            levelInfo = "[FATAL]";
            break;
    }

    if(threadInfo.empty())
    {
        std::thread::id pid = std::this_thread::get_id();
        std::stringstream pidsst;
        pidsst << pid;
        threadInfo = pidsst.str();
    }

    time_t now = time(NULL);
    struct tm* pt = localtime(&now);
    char stime[64];
    strftime(stime, sizeof stime, "[%Y-%m-%d %H:%M:%S]", pt);
    timeInfo = stime;
    res = timeInfo + levelInfo + " [ThreadID:" + threadInfo + "] [" + callInfo + "] " + detail + "\n";
    if(m_bTruncate)
        res = res.substr(0, m_iTruncateCount);
    return res;
}

void LogBase::outLog(LOG_LEVEL l, std::string content)
{
    if(m_eLevel <= l)
    {
        if(!m_bToFile)
            std::cout << content;
        else
        {
            utils::writeToFile(m_fStream, content);
            //当日志文件超过大小后自动截断
            m_fStream->seekg(0, std::ios::end);
            int size = m_fStream->tellg();
            std::cout << "当前文件大小：" << size << "Bytes" << std::endl;
            if (size >= m_iMaxFileSize)
            {
                m_fStream->close();
                long long now = utils::getMicroseconds();
                std::stringstream newLogName;
                newLogName << now << ".log";
                m_fStream->open(newLogName.str(), std::ios::in | std::ios::app);
                m_logName = newLogName.str();
            }

        }
    }
}

// [2022-02-12 23:22:11:0111] [INFO] [ThreadID:] [调用栈] 具体信息
void SyncLogger::info(std::string detail, std::string callInfo, std::string threadInfo)
{
    outLog(INFO, getOutputInfo(INFO, detail, threadInfo, callInfo));
}

void SyncLogger::debug(std::string detail, std::string callInfo, std::string threadInfo) {
    outLog(DEBUG, getOutputInfo(DEBUG, detail, threadInfo, callInfo));
}

void SyncLogger::warning(std::string detail, std::string callInfo, std::string threadInfo) {
    outLog(WARNING, getOutputInfo(WARNING, detail, threadInfo, callInfo));
}

void SyncLogger::error(std::string detail, std::string callInfo, std::string threadInfo) {
    outLog(ERROR, getOutputInfo(ERROR, detail, threadInfo, callInfo));
}

void SyncLogger::fatal(std::string detail, std::string callInfo, std::string threadInfo) {
    outLog(FATAL, getOutputInfo(FATAL, detail, threadInfo, callInfo));
}

void SyncLogger::log(std::string detail, std::string callInfo, std::string threadInfo, LOG_LEVEL l) {
    switch (l) {
        case LogBase::DEBUG:
            debug(detail, callInfo, threadInfo);
            break;
        case LogBase::INFO:
            info(detail, callInfo, threadInfo);
            break;
        case LogBase::WARNING:
            warning(detail, callInfo, threadInfo);
            break;
        case LogBase::ERROR:
            error(detail, callInfo, threadInfo);
            break;
        case LogBase::FATAL:
            fatal(detail, callInfo, threadInfo);
            break;
    }
}

void writeLogProc(AsyncLogger* logger)
{
    while(1)
    {
        AsyncLogger::LogItem logItem;
        {
            //加锁logBuffer
            std::unique_lock<std::mutex> lock(AsyncLogger::mutex_logBuffer);
            while(AsyncLogger::logBuffer.empty())
            {
                AsyncLogger::mutex_cv.wait(lock);
            }
            logItem = AsyncLogger::logBuffer.front();
            AsyncLogger::logBuffer.erase(AsyncLogger::logBuffer.begin());
        }
        logger->outLog(logItem.iLevel, logItem.iContent);
    }
}

//初始化静态变量
std::vector<AsyncLogger::LogItem>       AsyncLogger::logBuffer;
std::mutex                              AsyncLogger::mutex_logBuffer;
std::condition_variable                 AsyncLogger::mutex_cv;
std::vector<std::thread*>               AsyncLogger::writeThreads;

AsyncLogger* AsyncLogger::getAsyncLogger(LOG_LEVEL l, std::string outputFile, bool toFile, bool truncate, int numThreads)
{
    static AsyncLogger* asyncLogger;
    std::string logFile = outputFile;
    if(asyncLogger == nullptr)
    {
        if(toFile)
        {
            if (logFile.empty())
            {
                time_t now = time(NULL);
                std::stringstream fileName;
                fileName << now << ".log";
                logFile = fileName.str();
            }
        }
        asyncLogger = new AsyncLogger(l, logFile, toFile, truncate);
        asyncLogger->m_logName = logFile;
    }
    if(AsyncLogger::writeThreads.size() == 0)
    {
        for (int i = 0; i < numThreads; ++i)
        {
            std::thread* wt = new std::thread(writeLogProc, asyncLogger);
            AsyncLogger::writeThreads.push_back(wt);

        }
    }
    return asyncLogger;
}

void AsyncLogger::addLogToBuffer(LOG_LEVEL l, std::string logContent)
{
    LogItem logItem = {l, logContent};
    {
        //加锁logBuffer
        std::unique_lock<std::mutex> lock(AsyncLogger::mutex_logBuffer);
        AsyncLogger::logBuffer.push_back(logItem);
        mutex_cv.notify_one();
    }
}

void AsyncLogger::debug(std::string detail, std::string callInfo, std::string threadInfo)
{
    std::string logInfo = getOutputInfo(DEBUG, detail, threadInfo, callInfo);
    addLogToBuffer(DEBUG, logInfo);
}

void AsyncLogger::info(std::string detail, std::string callInfo, std::string threadInfo)
{
    addLogToBuffer(INFO, getOutputInfo(INFO, detail, threadInfo, callInfo));
}

void AsyncLogger::warning(std::string detail, std::string callInfo, std::string threadInfo) {
    addLogToBuffer(WARNING, getOutputInfo(WARNING, detail, threadInfo, callInfo));
}

void AsyncLogger::error(std::string detail, std::string callInfo, std::string threadInfo) {
    addLogToBuffer(ERROR, getOutputInfo(ERROR, detail, threadInfo, callInfo));
}

void AsyncLogger::fatal(std::string detail, std::string callInfo, std::string threadInfo) {
    addLogToBuffer(FATAL, getOutputInfo(FATAL, detail, threadInfo, callInfo));
}