#include "stdafx.h"

#include "Timer.h"
#include <thread>
//#include <string>

Timer::Timer()
{
    myLastTimePoint = std::chrono::high_resolution_clock::now();
    myTotalTime = 0.0;
    myDeltaTime = 0.0;
}

void Timer::Update()
{
    auto currentTimePoint = std::chrono::high_resolution_clock::now();

    myDeltaTime = std::chrono::duration<float>(currentTimePoint - myLastTimePoint).count();
    myTotalTime += myDeltaTime;

    myLastTimePoint = currentTimePoint;
 }
