#include "timer.h"

Timer::Timer(double updateTime)
{
    update_time = updateTime;
    elapsed_msecs = 0;
    start_clock = std::clock();
    current_clock = start_clock;
}

Timer::~Timer()
{
    //cleanup
}

void Timer::reset()
{
    elapsed_msecs = 0;
    start_clock = std::clock();
    current_clock = start_clock;
}

void Timer::reset(double newUpdateTime)
{
    update_time = newUpdateTime;
    elapsed_msecs = 0;
    start_clock = std::clock();
    current_clock = start_clock;
}

bool Timer::isTime()
{
    current_clock = std::clock();
    elapsed_msecs = 1000 * double(current_clock - start_clock) / CLOCKS_PER_SEC;
    if (elapsed_msecs > update_time)
    {
        start_clock = current_clock;
        return true;
    }
    else
        return false;
}

double Timer::getElapsedTime()
{
    current_clock = std::clock();
    elapsed_msecs = 1000 * double(current_clock - start_clock) / CLOCKS_PER_SEC;
    return elapsed_msecs;
}

void wait(double ms)
{
    Timer wtime(ms);
    while (!wtime.isTime()) {;};
}