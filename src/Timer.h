//
// Created by Lijingxin on 2023/3/14.
//

#ifndef SOCKETSERVER_TIMER_H
#define SOCKETSERVER_TIMER_H

#include <atomic>
#include <stdint.h>
#include "Callback.h"

namespace net {
    ///计时器时间内部类
    class Timer {
    public:
        Timer(const TimerCallback &cb, TimeStamp when, int64_t interval, int64_t repeatCount = -1)
                : callback_(cb), expiration_(when), interval_(interval), repeatCount_(repeatCount),
                  sequence_(++s_numCreated_), canceled_(false) {}

        //std::move 是一个 C++ 标准库 中的函数模板，它可以将一个左值转换为一个右值引用123，从而可以触发移动构造函数或移动赋值运算符。
        // 这样可以避免不必要的拷贝操作，提高性能
        Timer(TimerCallback &&cb, TimeStamp when, int64_t interval)
                : callback_(std::move(cb)), expiration_(when), interval_(interval), repeatCount_(-1),
                  sequence_(++s_numCreated_),
                  canceled_(false) {}

        void run()
        {
            if(canceled_)
                return;
            callback_();
            if(repeatCount_ != -1)
            {
                --repeatCount_;
                if(repeatCount_ == 0)
                    return;
            }

            expiration_ += interval_;
        }

        bool isCanceled() const {return canceled_;}

        void cancel(bool off){canceled_ = off;}

        TimeStamp expiration() const { return expiration_; }
        int64_t getRepeatCount() const { return repeatCount_; }
        int64_t sequence() const { return sequence_; }

        static int64_t numCreated() { return s_numCreated_; }

    private:
        Timer(const Timer &rhs) = delete;

        Timer &operator=(const Timer &rhs) = delete;

    private:
        const TimerCallback callback_;
        TimeStamp expiration_;              //到期时间
        const int64_t interval_;
        int64_t repeatCount_;               //重复次数，-1代表一直重复
        const int64_t sequence_;
        bool canceled_;                     //是否处于取消状态

        static std::atomic<int64_t> s_numCreated_;
    };
}


#endif //SOCKETSERVER_TIMER_H
