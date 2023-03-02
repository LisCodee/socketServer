//
// Created by Lijingxin on 2023/3/1.
//
#include "log.h"

void testProc(AsyncLogger* logger)
{
    for(int i = 0; i < 10000; ++i){
        logger->info("这是一条INFO级别的日志", __func__ );
        logger->debug("这是一条DEBUG级别的日志",__func__);
        logger->warning("这是一条WARNING级别的日志",__func__);
        logger->error("这是一条ERROR级别的日志",__func__);
        logger->fatal("这是一条FATAL级别的日志",__func__);
    }
}

int main()
{
    AsyncLogger* logger = AsyncLogger::getAsyncLogger(LogBase::DEBUG, "", true, false, 4);
    std::vector<std::thread> ths;
    for (int i = 0; i < 4; ++i) {
        ths.push_back(std::thread(testProc, logger));
    }
    logger->join();
}

