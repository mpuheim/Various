#ifndef TIMER_H_
#define TIMER_H_

#include <ctime>

class Timer
{
private:
    std::clock_t start_clock;
    std::clock_t current_clock;
    double elapsed_msecs;
    double update_time;
public:
    Timer(double updateTime);
    virtual ~Timer();
    void reset();
    void reset(double newUpdateTime);
    bool isTime();
    double getElapsedTime(); //time since last update in ms
};

void wait(double ms);

#endif // TIMER_H_