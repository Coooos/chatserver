#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace placeholders; //_1 _2 _3
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,               // 时间循环
                       const InetAddress &listenAddr, // Ip+Port
                       const string &nameArg)         // 服务器的名字
    : _server(loop, listenAddr, nameArg),
      _loop(loop)
{
    // 回调函数： 事件发生时muduo库帮我们调用
    // 给服务器注册用户连接的创建和断开回调函数
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1)); // 参数占位符

    // 给服务器注册用户读写事件回调函数
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    // 设置服务器端的线程数量 1个I/O线程 3个worker线程
    _server.setThreadNum(4);
}

// 启动服务器
void ChatServer::start()
{
    _server.start(); // 启动服务器监听端口
}

// 专门处理用户的连接创建和断开
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected()) // 客户端断开连接
    {
        ChatService::instance()->clientCloseException(conn);
    }
}

// 专门处理用户的读写事件
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf); // 反序列化
    // 完全解耦网络模块的代码和业务模块的代码
    // 通过js【msgid】 获取-》业务hander -》conn js time
    int msgid = js["msgid"].get<int>();
    // cout << "msgid:" << msgid << endl;
    // 获取消息对应的处理器
    auto msgHandler = ChatService::instance()->getHandler(msgid);
    // 回调消息处理器，来执行相应的业务处理
    msgHandler(conn, js, time);
}
