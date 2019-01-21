#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <cstdio>
#include <ctime>

using namespace std;

class Timer
{
    public:
        Timer();
        bool secondsPast();
        double getSavedTime();
        double getCurrentTime();
        void setCurrentTime();
        bool overtime();
    protected:
    private:
        clock_t m_start;
        int m_seconds;
        double m_lastCallClock;
        double m_savedValue;
};

#endif // TIMER_H
