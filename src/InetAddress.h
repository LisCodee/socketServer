//
// Created by Lijingxin on 2023/3/14.
//

#ifndef SOCKETSERVER_INETADDRESS_H
#define SOCKETSERVER_INETADDRESS_H

#include "string"
#include "Platform.h"

namespace net{
    class InetAddress {
    public:
        ///Mostly used in TcpServer listening.
        explicit InetAddress(uint16_t port = 0, bool loopBackOnly = false);
        /// Constructs an endpoint with given ip and port.
        InetAddress(const std::string& ip, uint16_t port);
        /// Constructs an endpoint with given struct @c sockaddr_in
        InetAddress(const struct sockaddr_in& addr): addr_(addr){}

        std::string toIp() const;
        std::string toIpPort() const;
        uint16_t toPort() const;

        const struct sockaddr_in& getSockAddrInet() const{return addr_;}
        void setSockAddrInet(const struct sockaddr_in& addr) {addr_ = addr;}
        uint32_t ipNetEndian() const {return addr_.sin_addr.s_addr;}
        uint16_t portNetEndian() const {return addr_.sin_port;}

        ///resolve hostname to IP address, not changing port or sin_family
        static bool resolve(const std::string& hostname, InetAddress* result);

    private:
        struct sockaddr_in addr_;
    };
}



#endif //SOCKETSERVER_INETADDRESS_H
