//
// Created by Lijingxin on 2023/3/13.
//

#ifndef SOCKETSERVER_EVENTLOOP_H
#define SOCKETSERVER_EVENTLOOP_H

#include "net.h"
#include "AsyncLog.h"
#include "vector"
#include "TimeStamp.h"

namespace net {
    class Channel;
    ///
    /// Reactor, at most one per thread.
    ///
    /// This is an interface class, so don't expose too much details.
    class EventLoop {
    public:
        typedef std::function<void()> Functor;

        EventLoop();
        ~EventLoop();
        /**
         * 必须在创建对象的线程中调用
         */
        void loop();
        /**
         * 退出循环
         * 不是100%线程安全，最好使用shared_ptr<EventLoop>来保证线程安全
         */
        void quit();
        /**
         * Time when poll returns, usually means data arrival.
         * @return
         */
        TimeStamp pollReturnTime() const  { return pollReturnTime_;}
        /**
         * 暂时不清楚作用
         * @return
         */
        int64_t iteration() const {return iteration_;}
        /**
         * 立即在循环线程中运行回调函数。 它唤醒循环，并运行cb。 如果在同一个循环线程中，cb在函数内部运行。 可以从其他线程安全地调用。
         */
        void runInLoop(const Functor& cb);
        /**
         * 回调函数在循环线程中排队。 在完成loop后运行。 可以从其他线程安全地调用。
         */
        void queueInLoop(const Functor& cb);

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
        TimeStamp                           pollReturnTime_;
//        std::shared_ptr<Poller>             poller_;
//        std::shared_ptr<TimerQueue>         timerQueue_;
        int64_t                             iteration_;
#ifdef WIN32
        SOCKET m_wakeupFdListen;
        SOCKET m_wakeupFdSend;
        SOCKET m_wakeupFdRecv;
#else
        SOCKET m_wakeupFd;
#endif
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
