#include"Eventloop.h"
using namespace std;

EventLoop::EventLoop(int size):m_epoll(size)
{

}

void EventLoop::loop()
{
    while(true)
    {
        vector<SP_channel> event_fd = m_epoll.Poll();
        //cout<<"return event fd num:"<<event_fd.size()<<endl;
        /*-----------CPU单核心可以不用任务队列----------------*/
        // event_fd[i]->handle_event();
        // if(!event_fd[i]->Get_KeepAlive_State())
        // {
        //     event_fd[i]->Close();
        //     delete event_fd[i];
        // }
        ThreadPool::task_list->put_task(event_fd);
    }
}
//专门用于接收连接的epoll
void* EventLoop::MainLoop(void* ptr)
{
    EventLoop* EVLoop = (EventLoop*)ptr;
    while(true)
    { 
        vector<SP_channel> event_fd = EVLoop->m_epoll.Poll();
        for(int i = 0;i<event_fd.size();i++)
        {
            event_fd[i]->handle_event();
        }
    }
}
