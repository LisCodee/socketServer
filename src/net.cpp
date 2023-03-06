//
// Created by Lijingxin on 2023/3/2.
//

#include "net.h"
#include "iostream"

void printErrorMsg(std::string errInfo)
{
    std::cout << errInfo << "\terrno:" << getSocketError();
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
    int iOptValue = 1;
    int iOptLen = sizeof iOptValue;
    if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char*)&iOptValue, iOptLen) == -1)
    {
        reportNetErrorAndExit("setReuseAddr error" );
    }
}

void net::setReusePort(SOCKET sockFd, bool on) {
    int iOptValue = 1;
    int iOptLen = sizeof iOptValue;
    if(setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (char*)&iOptValue, iOptLen) == -1)
    {
        reportNetErrorAndExit("setReusePort error" );
    }
}

int net::connect(SOCKET sockFd, const struct sockaddr_in &addr) {
    if(::connect(sockFd, (struct sockaddr*)&addr, sizeof addr)!=0)
    {
        printErrorMsg("connect error");
    }
}

void net::bindOrDie(SOCKET sockFd, const struct sockaddr_in &addr) {
    if(::bind(sockFd, (struct sockaddr*)&addr, sizeof addr))
    {
        reportNetErrorAndExit("bind error");
    }
}

SOCKET net::accept(SOCKET sockFd, const sockaddr_in *addr) {
    SOCKET clientFd;
    int clientAddrLen = sizeof addr;
    if(::accept(sockFd, (struct sockaddr*)addr, &clientAddrLen) == INVALID_SOCKET)
    {
        printErrorMsg("Accept error");
    }
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