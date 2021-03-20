#include"channel.h"
#include"Epoll.h"
using namespace std;
channel::channel(int _fd,uint32_t _Sevent,Handle _read,Handle _write,TimeRound<channel>* _time_round):
epevent(),fd(_fd),Sevent(_Sevent),write(_write),read(_read),attach_epoll(NULL),time_round(_time_round)
{
    length = 4096;
    buff = new char[length];
    epevent.data.fd = _fd;
    epevent.events = Sevent;
}
int channel::handle_event()
{
    if(Revent & EPOLLIN)
    {
        int flag = read(shared_from_this(),buff,length);
        if(flag == 1)
        {
            cout<<"recv data from client: "<<fd<<" "<<buff<<endl;
        }
    }
    Revent = 0;
    return 0;
}

void channel::Add_New_Connect(int fd)
{
    SP_channel conn_socket(new channel(fd,EPOLLIN | EPOLLET,readn,writen,time_round));
    //channel* conn_socket = new channel(connfd,EPOLLIN | EPOLLET,readn,writen,false);
    attach_epoll->Epoll_Add(conn_socket,false);
}

void channel::Close()
{
    attach_epoll->Epoll_Del(fd);
}

channel::~channel()
{
    close(fd);
    LOG <<"client fd: "<<fd<<" has been closed";
    delete[] buff;
}

int channel::get_fd()
{
    return fd;
}

void channel::Set_Accept_Distribute_Epoll(Epoll* _attach_epoll)
{
    attach_epoll = _attach_epoll;
}

void channel::SeparateTimer()
{
    std::shared_ptr<TimeRoundItem<channel>> temp;
    //获得当前fd对应的定时器，如果有的话，需要先分离，lock()是线程安全的
    if(temp = wp_time_round_item.lock())
    {
        temp->reset();//将绑定到该定时器上的fd分离
    }
}

int channel::handle_close()
{
    //服务器端关闭连接
    std::shared_ptr<TimeRoundItem<channel>> temp;
    if(temp = wp_time_round_item.lock())
    {
        temp->reset();
    }
    Close();
}
