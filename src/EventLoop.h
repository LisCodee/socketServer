//
// Created by Lijingxin on 2023/3/13.
//

#ifndef SOCKETSERVER_EVENTLOOP_H
#define SOCKETSERVER_EVENTLOOP_H

#include "net.h"
#include "log.h"

namespace net {
    class Channel;

    class EventLoop {
    public:
        typedef std::function<void()> Functor;

        EventLoop();

        ~EventLoop();

    private:
        bool createWakeupFd();

        bool wakeup();

        bool handleRead();

        void doPendingFunctors();

        void abortNotInLoopThread();

    private:
        typedef std::vector<Channel *> ChannelList;

        bool looping;
        bool quit_;
        bool eventHandling_;
        bool callingPendingFunctors_;
        const std::thread::id threadId_;
//        Timestamp                           pollReturnTime_;
//        std::shared_ptr<Poller>             poller_;
//        std::shared_ptr<TimerQueue>         timerQueue_;
//        int64_t                             iteration_;
#ifdef WIN32
        SOCKET m_wakeupFdListen;
        SOCKET m_wakeupFdSend;
        SOCKET m_wakeupFdRecv;
#else
        SOCKET m_wakeupFd;
#endif
        LogBase *m_logger;
        std::shared_ptr<Channel>            wakeupChannel_;
        // scratch variables
        ChannelList                         activeChannels_;
        Channel*                            currentActiveChannel_;
        std::mutex                          mutex_;
        std::vector<Functor>                pendingFunctors_; // Guarded by mutex_
        Functor                             frameFunctor_;
    };
}


#endif //SOCKETSERVER_EVENTLOOP_H
