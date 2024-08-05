#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

//聊天服务器的主类
class ChatServer
{
public:
    //初始化聊天服务器对象
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);

    //启动服务
    void start();

private:
    //上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &conn);

    //上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &conn,
                            Buffer *buffer,
                            Timestamp time);

    /*
        TcpServer利用EventLoop（事件循环）来不断地检查和处理网络事件。
        EventLoop是一个基于Reactor模式的事件处理框架，它能够在单个线程中高效地处理多个并发事件。
    */
    TcpServer _server;  //组合的muduo库，实现服务器功能的类对象,是一个TCP服务器管理类
    EventLoop *_loop;   //指向事件循环对象的指针
};

#endif
