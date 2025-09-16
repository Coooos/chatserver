#include "/home/kuangjingmu/CHAT/include/server/chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>

// 处理服务器直接结束后，重置user的状态信息
void resetHandler(int)
{
    // 服务器异常退出，重置User表的状态信息
    // 把所有的online状态的用户，设置成offline
    ChatService::instance()->reset();
    exit(0);
}

int main()
{
    EventLoop loop; //类似 epoll
    InetAddress addr("127.0.0.1", 6000);// IP+Port
    ChatServer server(&loop,addr,"ChatServer");

    server.start(); //启动服务器
    loop.loop(); // 启动epoll事件循环 类似epoll_wait
    return 0;
}