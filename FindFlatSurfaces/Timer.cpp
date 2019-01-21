#include "Timer.h"

Timer::Timer()
{
    //ctor
    m_start = clock();
    m_seconds = 1;  //default refresh time
    m_lastCallClock = m_start;
}


bool Timer::secondsPast()
{
    double currentTime = clock();
    //if since last time it has been more than m_seconds, then return true
    if ( ( currentTime - m_lastCallClock) / (double) CLOCKS_PER_SEC > m_seconds)
    {
        m_lastCallClock = currentTime;
        return true;
    }
    return false;
}

void Timer::setCurrentTime()
{
    m_savedValue = clock();
}

double Timer::getSavedTime()
{
    return m_savedValue;
}

double Timer::getCurrentTime()
{
    return clock();
}

bool Timer::overtime()
{
    double currentTime = clock();
    if ( ( currentTime - m_savedValue) / (double) CLOCKS_PER_SEC > m_seconds)
    {
        return true;
    }
    return false;
}
