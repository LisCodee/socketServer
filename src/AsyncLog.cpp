//
// Created by Lijingxin on 2023/3/14.
//
#ifdef WIN32
#include <processthreadsapi.h>
#include "debugapi.h"
#endif

#include "AsyncLog.h"
#include "stdarg.h"
#include "iostream"

#define MAX_LINE_LENGTH 256
#define DEFAULT_ROLL_SIZE 10 * 1024 * 1024

bool AsyncLog::m_bToFile = true;                    //这个变量没用到，使用m_strFileName是否为空来判断是否需要写入文件
FILE* AsyncLog::m_hLogFile = NULL;
std::string AsyncLog::m_strFileName = "default";
std::string AsyncLog::m_strFileNamePid = "";
bool AsyncLog::m_bTruncateLongLog = false;
LOG_LEVEL AsyncLog::m_nCurrentLevel = LOG_LEVEL_DEBUG;
int AsyncLog::m_iFileRollSize = DEFAULT_ROLL_SIZE;
int AsyncLog::m_iCurrentWriteSize = 0;
bool AsyncLog::m_bExit = false;
bool AsyncLog::m_bRunning = false;
std::list<std::string> AsyncLog::m_listLinesToWrite;
std::shared_ptr<std::thread> AsyncLog::m_spWriteThread;
std::mutex AsyncLog::m_mutexWrite;
std::condition_variable AsyncLog::m_cvWrite;

void AsyncLog::uninit() {
    m_bExit = true;
    m_cvWrite.notify_one();

    if(m_spWriteThread->joinable())
        m_spWriteThread->join();

    if(m_hLogFile != nullptr)
    {
        fclose(m_hLogFile);
        m_hLogFile = nullptr;
    }
}

bool AsyncLog::init(const char *pszLogFileName, bool bTruncateLongLine, int nRollSize) {
    m_bTruncateLongLog = bTruncateLongLine;
    m_iFileRollSize = nRollSize;
    if(pszLogFileName == nullptr || pszLogFileName[0] == 0)
    {
        m_strFileName.clear();
    }
    else
        m_strFileName = pszLogFileName;
    //获取进程id，可以快速看到同一个进程的不同文件？
    char szPid[8];
#ifdef WIN32
    snprintf(szPid, sizeof szPid, "%05d", (int)::GetCurrentProcessId());
#else
    snprintf(szPid, sizeof szPid, "%05d", (int)::getpid());
#endif
    m_strFileNamePid = szPid;
    m_spWriteThread.reset(new std::thread(writeThreadProc));
    return true;
}

void AsyncLog::setLevel(LOG_LEVEL level) {
    m_nCurrentLevel = level;
}

bool AsyncLog::isRunning() {
    return m_bRunning;
}

bool AsyncLog::output(LOG_LEVEL nLevel, const char *pszFmt, ...) {
    if(nLevel != LOG_LEVEL_CRITICAL)
    {
        if(nLevel < m_nCurrentLevel)
            return false;
    }
    std::string strLine;
    makeLinePrefix(nLevel, strLine);

    //log正文
    std::string strLogMsg;
    //处理可变参数，
    va_list ap;
    va_start(ap, pszFmt);
    int nLogMsgLength = vsnprintf(NULL, 0, pszFmt, ap);
    va_end(ap);
    //分配空间，这里分配空间的原因是后面使用vsnprintf需要将string转为char*
    if(int(strLogMsg.capacity()) < nLogMsgLength+1)
    {
        strLogMsg.resize(nLogMsgLength+1);
    }
    va_list aq;
    va_start(aq, pszFmt);
    vsnprintf((char*)strLogMsg.data(), strLogMsg.capacity(), pszFmt, aq);
    va_end(aq);

    std::string strMsgFormal;
    strMsgFormal.append(strLogMsg, nLogMsgLength);

    //长日志截断处理
    if(m_bTruncateLongLog)
    {
        strMsgFormal = strMsgFormal.substr(0, MAX_LINE_LENGTH);
    }
    strLine += strMsgFormal;
    //如果不是输出到控制台，在末尾加上换行符
    if(!m_strFileName.empty())
    {
        strLine += "\n";
    }

    if(nLevel != LOG_LEVEL_FATAL)
    {
        std::lock_guard<std::mutex> lockGuard(m_mutexWrite);
        m_listLinesToWrite.push_back(strLine);
        m_cvWrite.notify_one();
    }
    else
    {
        //如果是fatal级别日志，采用同步写，以便程序能够及时结束
        std::cout << strLine << std::endl;
#ifdef _WIN32
        OutputDebugStringA(strLine.c_str());
        OutputDebugStringA("\n");
#endif
        if(!m_strFileName.empty())
        {
            //如果需要写文件
            if(m_hLogFile == nullptr)
            {
                //需要新建文件
                char szNow[64];
                time_t now = time(NULL);
                tm time;
#ifdef _WIN32
                localtime_s(&time, &now);
#else
                localtime_r(&now, &time);
#endif
                strftime(szNow, sizeof szNow, "%Y%m%d%H%M%S", &time);

                std::string strNewFileName(m_strFileName);
                strNewFileName += ".";
                strNewFileName += szNow;
                strNewFileName += ".";
                strNewFileName += m_strFileNamePid;
                strNewFileName += ".log";
                if(!createNewFile(strNewFileName.c_str()))
                {
                    //createFile failed
                    return false;
                }
            }
            writeToFile(strLine);
        }

        if(nLevel == LOG_LEVEL_FATAL)
        {
            //让程序主动崩溃
            crash();
        }
    }
    return true;
}

bool AsyncLog::output(LOG_LEVEL nLevel, const char *pszFileName, int nLineNo, const char *pszFmt, ...) {
    if(nLevel != LOG_LEVEL_CRITICAL)
    {
        if(nLevel < m_nCurrentLevel)
            return false;
    }
    std::string strLine;
    makeLinePrefix(nLevel, strLine);

    //函数签名
    char szFileName[512] = {0};
    snprintf(szFileName, sizeof szFileName, "[%s:%d]", pszFileName, nLineNo);
    strLine += szFileName;

    //日志正文
    std::string strLogMsg;
    va_list ap;
    va_start(ap, pszFmt);
    int nLogMsgLength = vsnprintf(NULL, 0, pszFmt, ap);
    va_end(ap);
    //分配空间
    if((int)strLogMsg.capacity() < nLogMsgLength+1)
    {
        strLogMsg.resize(nLogMsgLength+1);
    }
    //填充内容
    va_list aq;
    va_start(aq, pszFmt);
    vsnprintf((char*)strLogMsg.data(), sizeof strLogMsg, pszFmt, aq);
    va_end(aq);
    //string内容正确但length不对，恢复其length？
    std::string strMsgFormal;
    strMsgFormal.append(strLogMsg.c_str(), nLogMsgLength);
    //长日志截断
    if(m_bTruncateLongLog)
    {
        strMsgFormal = strMsgFormal.substr(0, MAX_LINE_LENGTH);
    }
    strLine += strMsgFormal;
    if(!m_strFileName.empty())
    {
        strLine += "\n";
    }
    if(m_nCurrentLevel != LOG_LEVEL_FATAL)
    {
        std::lock_guard<std::mutex> lockGuard(m_mutexWrite);
        m_listLinesToWrite.push_back(strLine);
        m_cvWrite.notify_one();
    }
    else
    {
        //采用同步写日志，以便使程序结束
        std::cout << strLine << std::endl;
#ifdef _WIN32
        OutputDebugStringA(strLine.c_str());
        OutputDebugStringA("\n");
#endif
        if(!m_strFileName.empty())
        {
            if(m_hLogFile == nullptr)
            {
                char szNow[64];
                time_t now = time(NULL);
                tm time;
#ifdef _WIN32
                localtime_s(&time, &now);
#else
                localtime_r(&now, &time);
#endif
                strftime(szNow, sizeof szNow, "%Y%m%d%H%M%S", &time);
                std::string strNewFileName(m_strFileName);
                strNewFileName += ".";
                strNewFileName += szNow;
                strNewFileName += ".";
                strNewFileName += m_strFileNamePid;
                strNewFileName += ".log";
                if(!createNewFile(strNewFileName.c_str()))
                    return false;
            }
            writeToFile(strLine);
        }
        if(nLevel == LOG_LEVEL_FATAL)
            crash();
    }
    return true;
}

/*
bool AsyncLog::outputBinary(unsigned char *buffer, size_t size) {
    //std::string strBinary;
    std::ostringstream os;

    static const size_t PRINTSIZE = 512;
    char szbuf[PRINTSIZE * 3 + 8];

    size_t lsize = 0;
    size_t lprintbufsize = 0;
    int index = 0;
    os << "address[" << long(buffer) << "] size[" << size << "] \n";
    while (true)
    {
        memset(szbuf, 0, sizeof(szbuf));
        if (size > lsize)
        {
            lprintbufsize = (size - lsize);
            lprintbufsize = lprintbufsize > PRINTSIZE ? PRINTSIZE : lprintbufsize;
            FormLog(index, szbuf, sizeof(szbuf), buffer + lsize, lprintbufsize);
            size_t len = strlen(szbuf);
            os << szbuf;
            lsize += lprintbufsize;
        }
        else
        {
            break;
        }
    }

    std::lock_guard<std::mutex> lock_guard(m_mutexWrite);
    m_listLinesToWrite.push_back(os.str());
    m_cvWrite.notify_one();
    return true;
}
 */

void AsyncLog::getTime(char *pszTime, int nTimeStrLength) {
    struct timeb tp;
    ftime(&tp);

    time_t now = tp.time;
    tm time;
#ifdef _WIN32
    localtime_s(&time, &now);
#else
    localtime_r(&now, &time);
#endif
    snprintf(pszTime, nTimeStrLength, "%04d-%02d-%02d %02d:%02d:%02d:%03d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, tp.millitm);
}

bool AsyncLog::createNewFile(const char *pszLogFileName) {
    if(m_hLogFile != nullptr)
    {
        fclose(m_hLogFile);
    }
    m_hLogFile = fopen(pszLogFileName, "w+");
    return m_hLogFile != nullptr;
}

bool AsyncLog::writeToFile(const std::string &data) {
    //为了防止长文件一次性写不完，放在一个循环里面分批写
    std::string strLocal(data);
    int ret = 0;
    while(true)
    {
        ret = fwrite(strLocal.c_str(), 1, strLocal.length(), m_hLogFile);
        if(ret < 0)
            return false;
        else if (ret <= (int)strLocal.length())
        {
            strLocal.erase(0, ret);
        }
        if(strLocal.empty())
            break;
    }
    fflush(m_hLogFile);
    return true;
}

void AsyncLog::makeLinePrefix(LOG_LEVEL nLevel, std::string &strPrefix) {
    //级别
    strPrefix = "[INFO]";
    if (nLevel == LOG_LEVEL_DEBUG)
        strPrefix = "[DEBUG]";
    else if (nLevel == LOG_LEVEL_WARNING)
        strPrefix = "[WARN]";
    else if (nLevel == LOG_LEVEL_ERROR)
        strPrefix = "[ERROR]";
    else if (nLevel == LOG_LEVEL_FATAL)
        strPrefix = "[FATAL]";
    else if (nLevel == LOG_LEVEL_CRITICAL)
        strPrefix = "[CRITICAL]";
    char szNow[64] = {0};
    getTime(szNow, sizeof szNow);
    strPrefix += "[";
    strPrefix += szNow;
    strPrefix += "]";

    //当前线程信息
    char szThreadId[32] = {0};
#ifdef WIN32
    DWORD threadId = ::GetCurrentThreadId();
#else
    int threadId = syscall(SYS_gettid);
#endif
    snprintf(szThreadId, sizeof szNow, "[%d]", (int)threadId);
    strPrefix += szThreadId;
}

void AsyncLog::crash() {
    char* p = nullptr;
    *p = 0;
}

void AsyncLog::writeThreadProc() {
    m_bRunning = true;

    while(true)
    {
        if(!m_strFileName.empty())
        {
            if(m_hLogFile == nullptr || m_iCurrentWriteSize >= m_iFileRollSize)
            {
                //需要新建文件
                m_iCurrentWriteSize = 0;
                char szNow[64];
                time_t now = time(NULL);
                tm time;
#ifdef _WIN32
                localtime_s(&time, &now);
#else
                localtime_r(&now, &time);
#endif
                strftime(szNow, sizeof(szNow), "%Y%m%d%H%M%S", &time);

                std::string strNewFileName(m_strFileName);
                strNewFileName += ".";
                strNewFileName += szNow;
                strNewFileName += ".";
                strNewFileName += m_strFileNamePid;
                strNewFileName += ".log";
                if (!createNewFile(strNewFileName.c_str()))
                    return;
            }
        }

        std::string strLine;
        {
            std::unique_lock<std::mutex> guard(m_mutexWrite);
            while(m_listLinesToWrite.empty())
            {
                if(m_bExit)
                    return;
                m_cvWrite.wait(guard);
            }
            strLine = m_listLinesToWrite.front();
            m_listLinesToWrite.pop_front();
        }
        std::cout << strLine;
        if(!m_strFileName.empty())
        {
            if(!writeToFile(strLine))
                return;
            m_iCurrentWriteSize += strLine.length();
        }
    }
    m_bRunning = false;
}

