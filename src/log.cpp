//
// Created by Lijingxin on 2023/3/1.
//
#include "log.h"
#include <iostream>
#include "ctime"
#include "utils.h"

SyncLogger* SyncLogger::getSyncLogger(LOG_LEVEL l, std::string outputFile, bool toFile, bool truncate)
{
    std::string logFile = outputFile;
    if(syncLogger == nullptr)
    {
        if(toFile)
        {
            if (logFile.empty())
            {
                time_t now = time(NULL);
                logFile = now + ".log";
            }
        }
        syncLogger = new SyncLogger(l, logFile, toFile, truncate);
    }
    return syncLogger;
}

std::string SyncLogger::getOutputInfo(SyncLogger::LOG_LEVEL l, std::string detail, std::string threadInfo, std::string callInfo)
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
    time_t now = time(NULL);
    struct tm* pt = localtime(&now);
    char stime[64];
    strftime(stime, sizeof stime, "%Y-%m-%d %H:%M:%S", pt);
    timeInfo = stime;
    res = timeInfo + levelInfo + " [ThreadID:" + threadInfo + " [" + callInfo + "] " + detail + "\n";
    if(m_bTruncate)
        res = res.substr(0, m_iTruncateCount);
    return res;
}

void SyncLogger::outLog(LOG_LEVEL l, std::string content)
{
    if(m_eLevel <= l)
    {
        if(!m_bToFile)
            std::cout << content;
        else
            utils::writeToFile(m_fStream, content);
    }
}

// [2022-02-12 23:22:11:0111] [INFO] [ThreadID:] [调用栈] 具体信息
void SyncLogger::info(std::string detail, std::string threadInfo, std::string callInfo)
{
    outLog(INFO, getOutputInfo(INFO, detail, threadInfo, callInfo));
}

void SyncLogger::debug(std::string detail, std::string threadInfo, std::string callInfo) {
    outLog(DEBUG, getOutputInfo(DEBUG, detail, threadInfo, callInfo));
}

void SyncLogger::warning(std::string detail, std::string threadInfo, std::string callInfo) {
    outLog(WARNING, getOutputInfo(WARNING, detail, threadInfo, callInfo));
}

void SyncLogger::error(std::string detail, std::string threadInfo, std::string callInfo) {
    outLog(ERROR, getOutputInfo(ERROR, detail, threadInfo, callInfo));
}

void SyncLogger::fatal(std::string detail, std::string threadInfo, std::string callInfo) {
    outLog(FATAL, getOutputInfo(FATAL, detail, threadInfo, callInfo));
}

void SyncLogger::log(std::string detail, std::string threadInfo, std::string callInfo, LOG_LEVEL l) {
    switch (l) {
        case LogBase::DEBUG:
            debug(detail, threadInfo, callInfo);
            break;
        case LogBase::INFO:
            info(detail, threadInfo, callInfo);
            break;
        case LogBase::WARNING:
            warning(detail, threadInfo, callInfo);
            break;
        case LogBase::ERROR:
            error(detail, threadInfo, callInfo);
            break;
        case LogBase::FATAL:
            fatal(detail, threadInfo, callInfo);
            break;
    }
}