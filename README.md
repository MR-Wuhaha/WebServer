#高性能Web服务器
1、采用Reactor模型，将建立连接与连接管理分为两个Epoll处理。<br>
2、采用时间轮管理创建的长连接<br>
3、日志采用双缓冲队列，并在写日志前用缓存保存<br>
4、实现了线程池处理连接接收的事件，并且采用生产者消费者模型管理任务，用双缓冲队列实现添加和消费任务之间的分离<br>
5、目前只能解析简单的http连接请求<br>


###代码的使用方法：
cd WebServer<br>
mkdir build<br>
cd build/<br>
cmake ..<br>
make<br>
./Mserver<br>
