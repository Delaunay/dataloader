#ifndef DATALOADER_UTILS_HEADER_H
#define DATALOADER_UTILS_HEADER_H

#include <chrono>
#include <ratio>
#include <cmath>

#include <mutex>
#include <condition_variable>
#include <cstdio>


template<typename T, typename ... Args>
void print_log(const char * fmt, const T& a, const Args& ... args) {
    printf(fmt, a, args...);
    printf("\n");
    fflush(stdout);
}

inline
void print_log(const char * fmt) {
    printf("%s\n", fmt);
    fflush(stdout);
}

#define DLOG(...) print_log(__VA_ARGS__)
#define ELOG(...) print_log(__VA_ARGS__)


class TimeIt {
  public:
    using TimePoint       = std::chrono::high_resolution_clock::time_point;
    using Clock           = std::chrono::high_resolution_clock;
    TimePoint const start = Clock::now();

    double stop() const {
        TimePoint end = Clock::now();
        std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        return time_span.count();
    }

    TimeIt operator=(TimeIt p){
        return TimeIt(p);
    }

    TimeIt(const TimeIt& p):
        start(p.start)
    {}

    TimeIt() = default;
};

struct StatStream {
    StatStream &operator+=(double val) {
        count += 1;
        sum += val;
        sum2 += val * val;
        return *this;
    }

    bool less_than(StatStream const &stats) const { return mean() < stats.mean(); }
    bool greater_than(StatStream const &stats) const { return mean() < stats.mean(); }

    bool equal(StatStream const &) const { return false; }
    bool different(StatStream const &) const { return true; }

    double mean() const { return sum / double(count); }
    double var() const { return sum2 / double(count) - mean() * mean(); }
    double sd() const { return sqrt(var()); }

    double sum        = 0;
    double sum2       = 0;
    std::size_t count = 0;
};

class Semaphore
{
private:
    std::mutex _mutex;
    std::condition_variable _condition;
    std::size_t _count = 0; // Initialized as locked.

public:
    Semaphore(std::size_t resource_count):
     _count(resource_count)
    {}

    void notify() {
        std::lock_guard<std::mutex> lock(_mutex);
        ++_count;
        _condition.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(_mutex);

        while(!_count)
            _condition.wait(lock);

        --_count;
    }

    bool try_wait() {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_count) {
            --_count;
            return true;
        }
        return false;
    }
};

#else
#undef DLOG
#undef ELOG
#define DLOG(...) print_log(__VA_ARGS__)
#define ELOG(...) print_log(__VA_ARGS__)
#endif
