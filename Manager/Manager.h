#pragma once

#include <thread>
#include <chrono>
#include <atomic>

class Manager
{
public:

    Manager();

    virtual ~Manager();

    /*
        Initialize and start thread
    */

    void init();

    /*
        Wait for thread exit
    */

    void thread_join();

protected:

    /*
        Thread loop period
    */

    std::chrono::milliseconds m_period;

    /*
        Thread running flag
    */

    std::atomic<bool> m_threadRunning;

    /*
        Worker thread
    */

    std::thread m_loopThread;

protected:

    /*
        Thread entry
    */

    void loopFcn();

    /*
        Virtual interfaces
    */

    virtual void start();

    virtual void stop();

    virtual void process();

    virtual void waitFor();
};