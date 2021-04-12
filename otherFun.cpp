#include"otherFun.h"
#include"Epoll.h"
#include"channel.h"
#include"Log.h"
using namespace std;
int Maccept(SP_channel _channel,char *buff,int length)
{
    while(true)
    {
        struct sockaddr_in Client_addr;
        memset(&Client_addr,0,sizeof(Client_addr));
        socklen_t Client_addr_len = sizeof(Client_addr);
        int connfd = accept(_channel->fd,(struct sockaddr*)&Client_addr,&Client_addr_len);
        if(connfd < 0)
        {
            if(errno == EAGAIN)
            {
                break;
            }
            else if(errno == EINTR)
            {
                continue;
            }
        }
        else if(connfd > 0)
        {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET,&Client_addr.sin_addr.s_addr,client_ip,sizeof(client_ip));
#if LOG_FLAG
            LOG<<"accept connect from : " + string(client_ip) + " port:" + to_string(Client_addr.sin_port) << " ClientFd:" << connfd;
#endif
            //cout<<"accept connect from : "<<client_ip << " port:" << Client_addr.sin_port <<endl;
            assert(set_noblock(connfd));
            _channel->Add_New_Connect(connfd);
        }
    }
    return 0;
}
int readn(SP_channel _channel,char *buff,int length)
{ 
    memset(buff,0,length);
    int read_len = 0;
    while(length > 0)
    {
        int rlen = recv(_channel->fd,buff,length,0);
        if(rlen < 0)
        {
            if(errno == EAGAIN)
            {
                break;
            }
            else if(errno == EINTR)
            {
                continue;
            }
        }
        else if(rlen > 0)
        {
            length = length - rlen;
            buff = buff + rlen;
            read_len += rlen;
        }
        else
        {
            //cout<<"client fd: "<<_channel->fd<<" closed or exception happend!"<<endl;
#if LOG_FLAG
            LOG<<"client fd: "+to_string(_channel->fd) + " closed or exception happend!";
#endif
            //对端关闭连接或者连接异常断开
            _channel->handle_close();
            return 0;
        }
    }
    return read_len;
}
int writen(SP_channel _channel,char *buff,int length)
{
    while(length > 0)
    {
        int wlen = send(_channel->fd,buff,length,0);
        if(wlen < 0)
        {
            if(errno == EAGAIN)
            {
                break;
            }
            else if(errno == EINTR)
            {
                continue;
            }
        }
        else if(wlen > 0)
        {
            length = length - wlen;
            buff = buff + wlen;
        }
        else
        {
            //cout<<"client fd: "<<_channel->fd<<" closed or exception happend!"<<endl;
#if LOG_FLAG
            LOG<<"client fd: "+to_string(_channel->fd) + " closed or exception happend!";
#endif
            //对端关闭连接或者连接异常断开
            _channel->handle_close();
            return 0;
        }
    }
    //cout<<"send to client:"<<_channel->fd<<" success!"<<endl;
    //delete (char*)_channel->epevent.data.ptr;
    //_channel->epevent.data.ptr = NULL;
    if(length == 0)
    {
        return -1;
    }
    return length;
}
int set_noblock(int sockfd)
{
    if(sockfd < 0)
    {
        cout<<"sockfd is invailed"<<endl;
        return -1;
    }
    int flags = fcntl(sockfd,F_GETFL,0);
    fcntl(sockfd,F_SETFL,flags | O_NONBLOCK);
    return 1;
}