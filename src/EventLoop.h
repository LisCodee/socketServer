//
// Created by Lijingxin on 2023/3/13.
//

#ifndef SOCKETSERVER_EVENTLOOP_H
#define SOCKETSERVER_EVENTLOOP_H
#include "net.h"
#include "log.h"

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    bool createWakeupFd();
    bool wakeup();
    bool handleRead();
    void handleOtherThings();
//    void queueInLoop(const Functor)
    friend std::string operator+(std::string s, EventLoop* loop);
private:
#ifdef WIN32
    SOCKET m_wakeupFdListen;
    SOCKET m_wakeupFdSend;
    SOCKET m_wakeupFdRecv;
#else
    SOCKET m_wakeupFd;
#endif
    LogBase* m_logger;

};


#endif //SOCKETSERVER_EVENTLOOP_H
