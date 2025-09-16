#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
private:
    TcpServer _server; // 组合muduo库的TcpServer类
    EventLoop *_loop;  // 事件循环指针 看作epoll

    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time);

public:
    // 初始化聊天服务器
    ChatServer(EventLoop *loop,               // 时间循环
               const InetAddress &listenAddr, // Ip+Port
               const string &nameArg);        // 服务器的名字

    void start();
};

#endif