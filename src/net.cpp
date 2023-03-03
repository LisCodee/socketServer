//
// Created by Lijingxin on 2023/3/2.
//

#include "net.h"
#include "iostream"

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
    std::cout << info << "\terrno:" << getSocketError();
#ifdef WIN32
    ::WSACleanup();
#endif
    exit(-1);
}

SOCKET net::createOrDie() {
    SOCKET sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockFd == INVALID_FD)
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

}