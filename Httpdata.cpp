#include"Httpdata.h"
#include"Epoll.h"
#include"Log.h"
Httpdata::Httpdata(int _fd,uint32_t _Sevent,Handle _read,Handle _write,TimeRound<channel>* _time_round):channel(_fd,_Sevent,_read,_write,_time_round)
{

}

int Httpdata::handle_event()
{
    if(Revent & EPOLLIN)
    {
        int flag = read(shared_from_this(),buff,length);
        if(flag == 1)
        {
            //cout<<"recv data from client: "<<fd<<" "<<buff<<endl;
            handle_http(buff);
            SeparateTimer();
            if(time_round != nullptr && Keep_Alive)
            {
                std::shared_ptr<TimeRoundItem<channel>> sp_time_round_item(new TimeRoundItem<channel>(5,bind(&Httpdata::Close,shared_from_this().get()),shared_from_this()));
                wp_time_round_item = std::weak_ptr<TimeRoundItem<channel>>(sp_time_round_item);
                SP_TimeRoundItemNode<channel>* sp_time_round_item_node = new SP_TimeRoundItemNode<channel>(sp_time_round_item);
                time_round->AddTimeRoundItemNode(sp_time_round_item_node);
            }
        }
    }
    Revent = 0;
    return 0;
}

void Httpdata::Add_New_Connect(int fd)
{
    //channel* conn_socket = new Httpdata(fd,EPOLLIN | EPOLLET,readn,writen,true);
    SP_channel conn_socket(new Httpdata(fd,EPOLLIN | EPOLLET,readn,writen,time_round));
    attach_epoll->Epoll_Add(conn_socket,false);
}

void Httpdata::handle_http(char *buff)
{
    int index = 0;
    Http_Handle *Recv_Http = new Http_Handle();
    handle_request_line(buff,index,Recv_Http);
    handle_request_head(buff,index,Recv_Http);
    if(Recv_Http->request_head.count("Connection") != 0 &&  
        (Recv_Http->request_head["Connection"] == "keep-alive" || 
        Recv_Http->request_head["Connection"] == "Keep-Alive") )
    {
        Keep_Alive = true;
    }
    string head = "";
    string body = "";
    //if(Recv_Http->URI == "222")
    if(Recv_Http->URI == "/")
    {
        body += "<html><title>Source Page</title>";
        body += "<body bgcolor=\"ffffff\">";
        body += "!Thank for visiting MRWu_haha's Server!";
        body += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />";
        body += "<br><a href=\"http://www.baidu.com\">百度一下</a>";
        body += "<br><a href=\"https://join.qq.com\">腾讯招聘</a>";
        body += "<br><a href=\"https://campus.kuaishou.cn\">快手招聘</a>";
        body += "<br><a href=\"https://uniportal.huawei.com\">华为招聘</a>";
        body += "<br><a href=\"https://campus.163.com\">网易招聘</a>";
        body += "<br><a href=\"https://careers.pinduoduo.com\">拼多多招聘</a>";
        body += "<br><a href=\"https://campus.alibaba.com\">阿里巴巴招聘</a>";
        body += "<br><a href=\"https://campus.bigo.sg\">BIGO招聘</a>";
        body += "</body></html>";
        head += Recv_Http->_version + " 200 OK\r\n";
        head += "Connection: Keep-Alive\r\n";
        head += "Content-type: text/html\r\n";
        head += "Content-Length: " + to_string(body.size()) + "\r\n";
        head += "\r\n";
        
    }
    else if(Recv_Http->URI == "/hello")
    {
        body += "<html><title>Hello</title>";
        body += "<body bgcolor=\"ffffff\">";
        body += "Thank for visiting this server!!!!";
        body += "<hr><em> MR_Wuhaha's WebServer </em>\n</body></html>";
        head += Recv_Http->_version + " 200 OK\r\n";
        head += "Connection: Keep-Alive\r\n";
        head += "Content-Type: text/html\r\n";
        head += "Content-Length: " + to_string(body.size()) + "\r\n";
        head += "\r\n";
    }
    else
    {
        body += "<html><title>Source Not Found</title>";
        body += "<body bgcolor=\"ffffff\">";
        body += "404 NOT_FOUND";
        body += "<hr><em> MR_Wuhaha's WebServer </em>\n</body></html>";
        head += Recv_Http->_version + " 404 NOTFOUND\r\n";
        head += "Connection: Keep-Alive\r\n";
        head += "Content-Type: text/html\r\n";
        head += "Content-Length: " + to_string(body.size()) + "\r\n";
        head += "\r\n";

    }
    delete Recv_Http;
    char send_word[4096];
    sprintf(send_word, "%s", head.c_str());
    write(shared_from_this(),send_word,strlen(send_word));
    sprintf(send_word, "%s", body.c_str());
    write(shared_from_this(),send_word,strlen(send_word));
}

void Httpdata::handle_request_line(char* buff,int& index,Http_Handle* Recv_Http)
{
    string temp = "";
    while(index <= strlen(buff) && buff[index] != ' ')
    {
        temp += buff[index];
        index++;
    }
    Recv_Http->Req = temp;
    index++;
    temp  = "";
    while(index <= strlen(buff) && buff[index] != ' ')
    {
        temp += buff[index];
        index++;
    }
    Recv_Http->URI = temp;
    index++;
    temp = "";
    while(index <= strlen(buff) && buff[index] != '\r' && buff[index] != '\n')
    {
        temp += buff[index];
        index++;
    }
    Recv_Http->_version = temp;
    index = index+2;
    LOG<<Recv_Http->Req+" "+Recv_Http->URI+" "+Recv_Http->_version;
    //cout<<Recv_Http->Req<<" "<<Recv_Http->URI<<" "<<Recv_Http->_version<<endl;
}

void Httpdata::handle_request_head(char* buff,int &index,Http_Handle* Recv_Http)
{
    while(index <= strlen(buff) && buff[index] != '\r' && buff[index] != '\n')
    {
        string key = "";
        string value = "";
        while(index <= strlen(buff) && buff[index] != ':')
        {
            key += buff[index];
            index++;
        }
        index = index+2;
        while(index <= strlen(buff) && buff[index] != '\r' && buff[index] != '\n')
        {
            value += buff[index];
            index++;
        }
        Recv_Http->request_head[key] = value;
        index = index+2;
    }
    index++;
}

bool Httpdata::Get_KeepAlive_State()
{
    return Keep_Alive;
}

Httpdata::~Httpdata()
{

}