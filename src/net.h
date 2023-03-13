//
// Created by Lijingxin on 2023/3/2.
//

#ifndef SOCKETSERVER_NET_H
#define SOCKETSERVER_NET_H

#include "Platform.h"
#include "string"


namespace net{

    /**
     * Wrapper of socket file descriptor
     */
    class Socket
    {
    public:
        //防止隐式转换
        explicit Socket(int sockfd):sockfd_(sockfd) {  }
        ~Socket(){ closesocket(sockfd_);}
        SOCKET fd() const {return sockfd_;}

        void bindAddress(const sockaddr_in& localAddr);

        void listen();

        int accept(const sockaddr_in& peerAddr);

        void setReuseAddr(bool on);

        void setReusePort(bool on);

        void setKeepAlive(bool on);

        void setTcpNoDelay(bool on);

    private:
        const SOCKET sockfd_;
    };

    SOCKET createOrDie();
    SOCKET createNonblockOrDie();
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
    int getSocketError();
//    void shutdownWrite(SOCKET sockfd);
}

#endif //SOCKETSERVER_NET_H
