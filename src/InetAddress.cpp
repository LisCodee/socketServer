//
// Created by Lijingxin on 2023/3/14.
//

#include "InetAddress.h"
#include "AsyncLog.h"
#include "Endian.h"

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;           //127.0.0.1


net::InetAddress::InetAddress(uint16_t port, bool loopBackOnly) {
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    in_addr_t ip = loopBackOnly ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = net::
}
