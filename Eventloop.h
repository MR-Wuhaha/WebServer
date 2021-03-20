#ifndef _EVENTLOOP
#define _EVENTLOOP
#include<iostream>
#include<vector>
#include"Epoll.h"
#include"ThreadPool.h"
class EventLoop
{
    public:
        Epoll m_epoll;
        EventLoop(int);
        void loop();
        static void* MainLoop(void*);
};

#endif
