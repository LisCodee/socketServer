//
// Created by Lijingxin on 2023/3/13.
//

#include "EventLoop.h"
#include "sstream"

bool net::EventLoop::createWakeupFd() {
#ifdef WIN32
    m_wakeupFdListen = net::createOrDie();
    m_wakeupFdSend = net::createOrDie();

    struct sockaddr_in bindAddr;
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddr.sin_port = 0;
    net::setReuseAddr(m_wakeupFdListen, true);
    net::bindOrDie(m_wakeupFdListen, bindAddr);
    net::listenOrDie(m_wakeupFdListen);

    struct sockaddr_in serverAddr;
    int serverAddrLen = sizeof serverAddr;
    //通过getsockname获取port
    if(getsockname(m_wakeupFdListen, (sockaddr*)&serverAddr, &serverAddrLen) < 0)
    {
        std::stringstream s;
        s << "Unable to bind address info, EventLoop:0x" << this;
        m_logger->fatal(s.str());
        return false;
    }
    int usePort = ntohs(serverAddr.sin_port);
    m_logger->info("wakeup fd use port:" + usePort);
    if(net::connect(m_wakeupFdSend, serverAddr) < 0)
    {
        m_logger->fatal(std::string("Unable to connect to wakeup peer, EventLoop:0x") + this);
        return false;
    }

    struct sockaddr_in clientAddr;
    m_wakeupFdRecv = net::accept(m_wakeupFdSend, &clientAddr);
    if(m_wakeupFdRecv < 0)
    {
        m_logger->fatal(std::string("Unable to accept wakeup peer, EventLoop:") + this);
        return false;
    }
    net::setNonblockAndCloseOnExec(m_wakeupFdSend);
    net::setNonblockAndCloseOnExec(m_wakeupFdRecv);
#else
    m_wakeupFd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(m_wakeupFd < 0)
    {
        m_logger->fatal(std::string("Unable to connect to wakeup peer"));
        return false;
    }
#endif
}

bool net::EventLoop::wakeup() {
    uint64_t one = 1;
#ifdef WIN32
    int32_t n = net::write(m_wakeupFdSend, &one, sizeof one);
#else
    int32_t n = net::write(m_wakeupFd, &one, sizeof one);
#endif
    if(n != sizeof one)
    {

        m_logger->fatal("wakeup() error:" + getSocketError());
        return false;
    }
    return true;
}

bool net::EventLoop::handleRead() {
    uint64_t one = 1;
#ifdef WIN32
    int n = net::read(m_wakeupFdRecv, &one, sizeof one);
#else
    int n = net::read(m_wakeupFd, &one, sizeof one);
#endif
    if(n != sizeof one)
    {
        m_logger->fatal("read error:" + getSocketError());
        return false;
    }
    return true;
}
