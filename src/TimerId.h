//
// Created by Lijingxin on 2023/3/14.
//

#ifndef SOCKETSERVER_TIMERID_H
#define SOCKETSERVER_TIMERID_H
#include "stdint.h"
namespace net
{
    class Timer;

    ///
    /// An opaque identifier, for canceling Timer.
    ///
    class TimerId
    {
    public:
        TimerId(): timer_(nullptr), sequence_(0)
        {
        }

        TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq)
        {
        }

        Timer* getTimer()
        {
            return timer_;
        }

        // default copy-ctor, dtor and assignment are okay

        friend class TimerQueue;

    private:
        Timer*      timer_;
        int64_t     sequence_;
    };

}
#endif //SOCKETSERVER_TIMERID_H
