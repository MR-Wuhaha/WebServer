#include"channel.h"
#include"Epoll.h"
using namespace std;
channel::channel(int _fd,uint32_t _Sevent,Handle _read,Handle _write,TimeRound<channel>* _time_round):
epevent(),fd(_fd),Sevent(_Sevent),write(_write),read(_read),attach_epoll(nullptr),accept_epoll(nullptr),time_round(_time_round)
{
    max_read_write_buff_length = 4096;
    read_buff = new char[max_read_write_buff_length];
    write_buff = new char[max_read_write_buff_length];
    read_length = 0;
    write_length = 0;
    epevent.data.fd = _fd;
    epevent.events = Sevent;
}
int channel::handle_event()
{
    if(Revent & EPOLLIN)
    {
        HandleRead();
    }
    else if(Revent & EPOLLOUT)
    {
        HandleWrite();
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
    delete[] read_buff;
    delete[] write_buff;
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

void channel::HandleRead()
{
    read_length = read(shared_from_this(),read_buff,max_read_write_buff_length);
}

void channel::HandleWrite()
{
    int wlen = write(shared_from_this(),write_buff,write_length);
    if(wlen > 0)
    {
        if(wlen < write_length)
        {
            memcpy(write_buff,write_buff + (write_length - wlen),wlen);
        }
        write_length = wlen;
        epevent.events = EPOLLOUT | EPOLLIN | EPOLLET;
        attach_epoll->Epoll_Mod(shared_from_this());
    }
    else if(wlen == -1)
    {
        epevent.events = EPOLLIN | EPOLLET;
        attach_epoll->Epoll_Mod(shared_from_this());
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
