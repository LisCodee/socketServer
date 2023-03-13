//
// Created by Lijingxin on 2023/3/2.
//

#include "net.h"
#include "iostream"
#include "log.h"
#include "sstream"

void printErrorMsg(std::string errInfo)
{
    SyncLogger* logger = SyncLogger::getSyncLogger(SyncLogger::INFO, "./log.txt", false);
    std::stringstream output;
    output << errInfo << "\terrno:" << getSocketError();
    logger->error(output.str());
}

#ifdef WIN32
void net::initSocketOrDie() {
    WORD wVersionRequested;
    WSADATA wsaData;
    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2,2);
    int err = ::WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {
        reportNetErrorAndExit("WSAStartup failed with error: ");
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        reportNetErrorAndExit("Could not find a usable version of Winsock.dll");
    }
}

void net::uninitSocket() {
    ::WSACleanup();
}
#endif

void net::reportNetErrorAndExit(std::string info)
{
    printErrorMsg(info);
#ifdef WIN32
    ::WSACleanup();
#endif
    exit(-1);
}

SOCKET net::createOrDie() {
    SOCKET sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockFd == INVALID_SOCKET)
    {
        reportNetErrorAndExit("create socket error.");
    }
}

void net::listenOrDie(SOCKET sockFd) {
    if(listen(sockFd, 0) < 0)
    {
        reportNetErrorAndExit("listen socket error");
    }
}

void net::setNonblockAndCloseOnExec(SOCKET sockFd) {
#ifdef WIN32
    u_long mode = 1;
    if(!ioctlsocket(sockFd, FIONBIO, &mode))
    {
        reportNetErrorAndExit("create nonblock socket error. ");
    }
#else
    int oldSocketFlag = fcntl(sockFd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if(fcntl(sockFd, F_SETFL, newSocketFlag) == -1)
    {
        reportNetErrorAndExit("create nonblock socket error. ");
    }
#endif
}

SOCKET net::createNonblockOrDie() {
    SOCKET sockFd = createOrDie();
    setNonblockAndCloseOnExec(sockFd);
    return sockFd;
}

void net::setReuseAddr(SOCKET sockFd, bool on) {
    if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof on) == -1)
    {
        reportNetErrorAndExit("setReuseAddr error" );
    }
}

void net::setReusePort(SOCKET sockFd, bool on) {
    if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof on) == -1)
    {
        reportNetErrorAndExit("setReusePort error" );
    }
}

int net::connect(SOCKET sockFd, const struct sockaddr_in &addr) {
    if(::connect(sockFd, (struct sockaddr*)&addr, sizeof addr)!=0)
    {
        printErrorMsg("connect error");
        return -1;
    }
    return 0;
}

void net::bindOrDie(SOCKET sockFd, const struct sockaddr_in &addr) {
    if(::bind(sockFd, (struct sockaddr*)&addr, sizeof addr))
    {
        reportNetErrorAndExit("bind error");
    }
}

SOCKET net::accept(SOCKET sockFd, const sockaddr_in *addr) {
    SOCKET clientFd = -1;
    int clientAddrLen = sizeof addr;
    if((clientFd = ::accept(sockFd, (struct sockaddr*)addr, &clientAddrLen)) == INVALID_SOCKET)
    {
        printErrorMsg("Accept error");
    }
    return clientFd;
}

int32_t net::read(SOCKET sockFd, void *buf, int32_t count) {
    int32_t readLen = recv(sockFd, (char*)buf, count, 0);
    if(readLen == 0)
    {
        //对端关闭连接
        close(sockFd);
        return 0;
    }else if(readLen < 0)
    {
        //阻塞或出错
        if(getSocketError() == EWOULDBLOCK)
        {

        }
        printErrorMsg("read error");
//        close(sockFd);
        return -1;
    }else
        return readLen;
}

int32_t net::write(SOCKET sockFd, const void *buf, int32_t count) {
    int32_t writeLen = send(sockFd, (char*)buf, count, 0);
    if(writeLen < 0)
    {
        if(getSocketError() == EWOULDBLOCK)
        {

        }
        printErrorMsg("write error");
//        close(sockFd);
        return -1;
    }else
        return writeLen;
}

void net::close(SOCKET sockFd) {
    closesocket(sockFd);
}

void net::toIp(char *buf, size_t size, const struct sockaddr_in &addr) {
    buf = inet_ntoa(addr.sin_addr);
}

void net::toIpPort(char *buf, size_t size, const struct sockaddr_in &addr) {
    toIp(buf, size, addr);
    short port = ntohs(addr.sin_port);
    strcat(buf, ":");
    sprintf(buf, "%d", port);
}

void net::fromIpPort(const char *ip, short port, struct sockaddr_in *addr) {
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip);
}

struct sockaddr_in net::getLocalAddr(SOCKET sockFd) {
    struct sockaddr_in localAddr;
    int len = sizeof localAddr;
    getsockname(sockFd, (struct sockaddr*)&localAddr, &len);
    return localAddr;
}

struct sockaddr_in net::getPeerAddr(SOCKET sockFd) {
    struct sockaddr_in peerAddr;
    int len = sizeof peerAddr;
    getpeername(sockFd, (struct sockaddr*)&peerAddr, &len);
    return peerAddr;
}

int net::getSocketError() {
#ifdef WIN32
    return ::WSAGetLastError();
#else
    return errno;
#endif
}

void net::Socket::bindAddress(const sockaddr_in &localAddr) {
    bindOrDie(sockfd_, localAddr);
}

void net::Socket::listen() {
    net::listenOrDie(sockfd_);
}

int net::Socket::accept(const sockaddr_in &peerAddr) {
    SOCKET peerfd = net::accept(sockfd_, &peerAddr);
    if(peerfd < 0){
        return INVALID_SOCKET;
    }
    return peerfd;
}

void net::Socket::setReuseAddr(bool on) {
    net::setReuseAddr(sockfd_, on);
}

void net::Socket::setReusePort(bool on) {
    net::setReusePort(sockfd_, on);
}

void net::Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
#ifdef WIN32
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
#else
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
#endif
}

void net::Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
#ifdef WIN32
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, sizeof optval);
#else
	::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
#endif
}

