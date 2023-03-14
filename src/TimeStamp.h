//
// Created by Lijingxin on 2023/3/14.
//

#ifndef SOCKETSERVER_TIMESTAMP_H
#define SOCKETSERVER_TIMESTAMP_H

#include <stdint.h>
#include <algorithm>
#include <string>


class TimeStamp {
public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    TimeStamp():microSecondsSinceEpoch_(0){}
    explicit TimeStamp(int64_t m):microSecondsSinceEpoch_(m){}

    TimeStamp& operator+=(TimeStamp& lhs)
    {
        this->microSecondsSinceEpoch_ += lhs.microSecondsSinceEpoch_;
        return *this;
    }

    TimeStamp& operator+=(int64_t lhs)
    {
        this->microSecondsSinceEpoch_ += lhs;
        return *this;
    }

    TimeStamp& operator-=(TimeStamp& lhs)
    {
        this->microSecondsSinceEpoch_ -= lhs.microSecondsSinceEpoch_;
        return *this;
    }

    TimeStamp& operator-=(int64_t lhs)
    {
        this->microSecondsSinceEpoch_ -= lhs;
        return *this;
    }

    void swap(TimeStamp& other)
    {
        int64_t temp = other.microSecondsSinceEpoch_;
        other.microSecondsSinceEpoch_ = this->microSecondsSinceEpoch_;
        this->microSecondsSinceEpoch_ = temp;
    }

    std::string toString() const;
    std::string toFormatString(bool showMicroseconds = true) const;
    bool valid() const {return microSecondsSinceEpoch_ > 0;}
    int64_t getMicroSecondsSinceEpoch() const{return microSecondsSinceEpoch_;}
    time_t getSecondSinceEpoch() const {return static_cast<time_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond);}
    static TimeStamp now();
    static TimeStamp invalid();

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.getMicroSecondsSinceEpoch() < rhs.getMicroSecondsSinceEpoch();
}

inline bool operator>(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.getMicroSecondsSinceEpoch() > rhs.getMicroSecondsSinceEpoch();
}

inline bool operator<=(TimeStamp lhs, TimeStamp rhs)
{
    return !(lhs > rhs);
}

inline bool operator>=(TimeStamp lhs, TimeStamp rhs)
{
    return !(lhs < rhs);
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.getMicroSecondsSinceEpoch() == rhs.getMicroSecondsSinceEpoch();
}

inline bool operator!=(TimeStamp lhs, TimeStamp rhs)
{
    return !(lhs == rhs);
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
inline double timeDifference(TimeStamp high, TimeStamp low)
{
    int64_t diff = high.getMicroSecondsSinceEpoch() - low.getMicroSecondsSinceEpoch();
    return static_cast<double> (diff) / TimeStamp::kMicroSecondsPerSecond;
}

inline TimeStamp addTime(TimeStamp timeStamp, int64_t microSeconds)
{
    return TimeStamp(timeStamp.getMicroSecondsSinceEpoch() + microSeconds);
}
#endif //SOCKETSERVER_TIMESTAMP_H
