//
// Created by Lijingxin on 2023/3/1.
//
#include "log.h"

int main()
{
    SyncLogger* logger = SyncLogger::getSyncLogger(LogBase::DEBUG, "", false, false);
    logger->info("这是一条INFO级别的日志");
}