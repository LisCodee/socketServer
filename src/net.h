//
// Created by Lijingxin on 2023/3/2.
//

#ifndef SOCKETSERVER_NET_H
#define SOCKETSERVER_NET_H

#ifdef WIN32
#include <winsock.h>
#include <string>

#else
#define INVALID_SOCKET -1
#endif


namespace net{
#ifdef WIN32
#define getSocketError() WSAGetLastError()
    typedef INT32 int32_t;
    /**
     * WSAStartup 函数启动进程对 Winsock DLL 的使用
     */
    void initSocketOrDie();
    /**
     * 进程退出时调用
     */
    void uninitSocket();
#else
#define closesocket(s) close(s)
#define getSocketError() errno
    typedef int SOCKET;
#endif
    /**
     * 创建socket，如果失败则退出
     * @return socketFd
     */
    SOCKET createOrDie();
    /**
     * 创建非阻塞socket，如果失败则退出
     * @return socketFd
     */
    SOCKET createNonblockOrDie();

    /**
     * 改变socket的属性为非阻塞
     * @param sockFd
     */
    void setNonblockAndCloseOnExec(SOCKET sockFd);

    void setReuseAddr(SOCKET sockFd, bool on);
    void setReusePort(SOCKET sockFd, bool on);

    int connect(SOCKET sockFd, const struct sockaddr_in& addr);
    void bindOrDie(SOCKET sockFd, const struct sockaddr_in& addr);
    void listenOrDie(SOCKET sockFd);
    SOCKET accept(SOCKET sockFd, const sockaddr_in* addr);

    int32_t read(SOCKET sockFd, void *buf, int32_t count);
    int32_t write(SOCKET sockFd, const void* buf, int32_t count);
    void close(SOCKET sockFd);

    void toIpPort(char* buf, size_t size, const struct sockaddr_in& addr);
    void toIp(char* buf, size_t size, const struct sockaddr_in& addr);
    void fromIpPort(const char* ip, short port, struct sockaddr_in* addr);

    /**
     * 获取socket绑定的地址信息
     * @param sockFd 需要获得socket描述符
     * @return 绑定的地址
     */
    struct sockaddr_in getLocalAddr(SOCKET sockFd);
    /**
     * 获取socket套接字远程的连接信息
     * @param sockFd 需要获取的套接字
     * @return 远程地址信息
     */
    struct sockaddr_in getPeerAddr(SOCKET sockFd);
    void reportNetErrorAndExit(std::string info);
}

#endif //SOCKETSERVER_NET_H
