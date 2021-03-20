#ifndef _CHANNEL
#define _CHANNEL
#include<iostream>
#include<sys/socket.h>
#include<functional>
#include<memory>
#include"TimeRound.h"
#include<sys/epoll.h>
#include"otherFun.h"
#include"Log.h"
class Epoll;
class channel;
class Timer_node;
typedef std::shared_ptr<channel> SP_channel;
typedef int(*Handle)(SP_channel,char*,int);
class channel:public enable_shared_from_this<channel>
{
    protected:
        int fd;
        uint32_t Revent;
        uint32_t Sevent;
        Handle read;
        Handle write;
        Epoll *accept_epoll;
        Epoll *attach_epoll;
        char* buff;
        int length;
        TimeRound<channel>* time_round;
        std::weak_ptr<TimeRoundItem<channel>> wp_time_round_item;
    public:
        epoll_event epevent;
        friend class Epoll;
        channel(int _fd,uint32_t _Sevent,Handle _read,Handle _write,TimeRound<channel>* _time_round);
        virtual ~channel();
        virtual int handle_event();
        virtual int handle_close();
        bool Get_KeepAlive_State();
        void Close();
        friend int Maccept(SP_channel _channel,char *buff,int length);
        friend int readn(SP_channel _channel,char *buff,int length);
        friend int writen(SP_channel _channel,char *buff,int length);
        void Set_Accept_Distribute_Epoll(Epoll*);
        virtual void Add_New_Connect(int);
        int get_fd();
        virtual void SeparateTimer();
};
#endif
