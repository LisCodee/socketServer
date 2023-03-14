//
// Created by Lijingxin on 2023/3/14.
//

#include "InetAddress.h"
#include "AsyncLog.h"
#include "Endian.h"
#include "net.h"

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;           //127.0.0.1


net::InetAddress::InetAddress(uint16_t port, bool loopBackOnly) {
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    in_addr_t ip = loopBackOnly ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = net::sockets::hostToNetwork32(ip);
}

net::InetAddress::InetAddress(const std::string &ip, uint16_t port) {
    memset(&addr_, 0, sizeof addr_);
    net::fromIpPort(ip.c_str(), port, &addr_);
}

std::string net::InetAddress::toIp() const {
    char szBuf[32] = {0};
    net::toIp(szBuf, sizeof szBuf, addr_);
    return szBuf;
}

std::string net::InetAddress::toIpPort() const {
    char szBuf[32] = {0};
    net::toIpPort(szBuf, sizeof szBuf, addr_);
    return szBuf;
}

uint16_t net::InetAddress::toPort() const {
    return htons(addr_.sin_port);
}

static thread_local char t_resolveBuffer[64 * 1024];

bool net::InetAddress::resolve(const std::string &hostname, net::InetAddress *result) {
    //使用gethostbyname，这个方法已被getaddrinfo替代
    struct hostent* phEntry;
    phEntry = gethostbyname(hostname.c_str());
    if(phEntry == NULL)
    {
        LOGE("resolve hostname error:%d", net::getSocketError());
        result = NULL;
        return false;
    }
    result->addr_.sin_addr.s_addr = *((unsigned long *)phEntry->h_addr_list[0]);
}
