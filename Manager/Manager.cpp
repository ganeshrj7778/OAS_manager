#include "Manager.h"

#include <iostream>

Manager::Manager()
{
    m_period =
        std::chrono::milliseconds(100);

    m_threadRunning = false;
}

Manager::~Manager()
{
    m_threadRunning = false;

    if(m_loopThread.joinable())
    {
        m_loopThread.join();
    }

    stop();
}

void Manager::init()
{
    m_threadRunning = true;

    start();

    m_loopThread = std::thread(&Manager::loopFcn, this);
}

void Manager::thread_join()
{
    if(m_loopThread.joinable())
    {
        m_loopThread.join();
    }
}

void Manager::start()
{
}

void Manager::stop()
{
}

void Manager::process()
{
}

void Manager::waitFor()
{
    std::this_thread::sleep_for(
        m_period);
}

void Manager::loopFcn()
{
    while(m_threadRunning)
    {
        process();

        waitFor();
    }

    std::cout << "Manager thread exited"
              << std::endl;
}