//
// Created by Lijingxin on 2023/3/2.
//

#ifndef SOCKETSERVER_CHANNEL_H
#define SOCKETSERVER_CHANNEL_H

#include <security.h>
#include "net.h"
#include "EventLoop.h"

class Channel {

public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(TimeStamp recvTime);
    int fd() const;
    int events() const;
    void setRevents(int revt);
    void addRevents(int revt);
    void removeEvents();
    bool isNoneEvent() const;

    bool enableRead();
    bool disableRead();
    bool enableWrite();
    bool disableWrite();
    bool disableAll();

    bool isWriting() const;

private:
    const int   m_fd;       //当前需要检测的事件
    int         m_events;   //处理后的事件
    int         m_revents;
};


#endif //SOCKETSERVER_CHANNEL_H
