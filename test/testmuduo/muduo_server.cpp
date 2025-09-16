/*
muduo网络库给用户提供了两个主要的类
Tcpserver:用于编写服务器程序
Tcpclient:用于编写客户端程序

epoll+线程池+回调函数
好处： 能够把网络的I/O操作和业务逻辑代码分开
用户只需要关心业务逻辑代码的编写
网络I/O操作由muduo网络库来完成
用户只需要注册回调函数，muduo网络库在适当的时机调用用户注册的回调函数
*/
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>

#include <functional> //需要使用绑定器
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders; //_1 _2 _3

class ChatServer
{
public:
    // 初始化tcpserver对象
    ChatServer(EventLoop *loop,               // 时间循环
               const InetAddress &listenAddr, // Ip+Port
               const string &nameArg)         // 服务器的名字
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        // 回调函数： 事件发生时muduo库帮我们调用
        // 给服务器注册用户连接的创建和断开回调函数
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1)); // 参数占位符

        // 给服务器注册用户读写事件回调函数
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        // 设置服务器端的线程数量 1个I/O线程 3个worker线程
        _server.setThreadNum(4);
    }

    void start()
    {
        _server.start(); // 启动服务器监听端口
    }

private:
    // 专门处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected()) // 新连接
        {
            cout << conn->peerAddress().toIpPort() << "->"
                 << conn->localAddress().toIpPort() << "state:online" << endl;
        }
        else // 断开连接
        {
            cout << conn->peerAddress().toIpPort() << "->"
                 << conn->localAddress().toIpPort() << "state:offline" << endl;
            conn->shutdown(); // 关闭写端
            // _loop->quit();//退出事件循环
        }
    }

    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buf,
                   Timestamp time)
    {
        string msg = buf->retrieveAllAsString();
        cout << "recv data:" << msg << "time:" << time.toString() << endl;
    }

    TcpServer _server; // 组合muduo库的TcpServer类
    EventLoop *_loop;  // 事件循环指针 看作epoll
};

int main()
{
    EventLoop loop; //类似 epoll
    InetAddress addr("127.0.0.1", 6000);// IP+Port
    ChatServer server(&loop,addr,"ChatServer");

    server.start(); //启动服务器
    loop.loop(); // 启动epoll事件循环 类似epoll_wait
    return 0;
}