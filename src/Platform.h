//
// Created by Lijingxin on 2023/3/13.
//

#ifndef SOCKETSERVER_PLATFORM_H
#define SOCKETSERVER_PLATFORM_H

#include <cstdint>

#ifdef WIN32

typedef int          socklen_t;
//typedef uint64_t     ssize_t;
typedef unsigned int in_addr_t;

#define  XPOLLIN         1
#define  XPOLLPRI        2
#define  XPOLLOUT        4
#define  XPOLLERR        8
#define  XPOLLHUP        16
#define  XPOLLNVAL       32
#define  XPOLLRDHUP      8192

#define  XEPOLL_CTL_ADD  1
#define  XEPOLL_CTL_DEL  2
#define  XEPOLL_CTL_MOD  3

#include <winsock2.h>
#include <Windows.h>
#include <io.h>     //_pipe
#include <fcntl.h>  //for O_BINARY

class NetworkInitializer {
public:
    NetworkInitializer();
    ~NetworkInitializer();

};

#else
typedef int SOCKET;
#define INVALID_SOCKET -1;
#define closesocket(s) close(s);

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <stdint.h>
#include <endian.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <inttypes.h>
#include <errno.h>
#include <dirent.h>


#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/eventfd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#endif

#endif //SOCKETSERVER_PLATFORM_H
