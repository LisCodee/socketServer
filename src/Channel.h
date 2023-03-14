//
// Created by Lijingxin on 2023/3/2.
//

#ifndef SOCKETSERVER_CHANNEL_H
#define SOCKETSERVER_CHANNEL_H

#include "net.h"
#include "EventLoop.h"

namespace net{
    class EventLoop;
    class Channel {

    public:
        typedef std::function<void()> EventCallback;
//        typedef std::function<void(Timestamp)> ReadEventCallback;

        Channel(EventLoop* loop, int fd);
        ~Channel();

//        void handleEvent(TimeStamp recvTime);
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
        static const int            kNoneEvent;
        static const int            kReadEvent;
        static const int            kWriteEvent;

        EventLoop*                  loop_;
        const int                   fd_;
        int                         events_;
        int                         revents_; // it's the received event types of epoll or poll
        int                         index_; // used by Poller.
        bool                        logHup_;

        std::weak_ptr<void>         tie_;           //std::shared_ptr<void>/std::shared_ptr<void>可以指向不同的数据类型
        bool                        tied_;
        //bool                        eventHandling_;
        //bool                        addedToLoop_;
//        ReadEventCallback           readCallback_;
        EventCallback               writeCallback_;
        EventCallback               closeCallback_;
        EventCallback               errorCallback_;
    };
}



#endif //SOCKETSERVER_CHANNEL_H
