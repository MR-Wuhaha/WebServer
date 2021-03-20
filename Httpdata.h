#ifndef _HTTPDATA
#define _HTTPDATE
#include"channel.h"
#include<unordered_map>

struct Http_Handle
{
    string Req;
    string _version;
    string URI;
    unordered_map<string,string> request_head;
    string request_body;
    Http_Handle()
    {
        Req = "";
        _version = "";
        URI = "";
    }
};

class Httpdata:public channel
{
    public:
        Httpdata(int _fd,uint32_t _Sevent,Handle _read,Handle _write,TimeRound<channel>* time_round);
        void handle_http(char* buff);
        void handle_request_line(char*,int&,Http_Handle*);
        void handle_request_head(char*,int&,Http_Handle*);
        bool Get_KeepAlive_State();
        virtual int handle_event();
        virtual void Add_New_Connect(int);
        friend int Maccept(SP_channel _channel,char *buff,int length);
        friend int readn(SP_channel _channel,char *buff,int length);
        friend int writen(SP_channel _channel,char *buff,int length);
        ~Httpdata();
    private:
        bool Keep_Alive;
};
#endif