#ifndef CHATSERVICE_HPP
#define CHATSERVICE_HPP 

#include "usermodel.hpp"
#include "offlinemessgemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include <muduo/net/TcpServer.h>
#include <unordered_map>
#include <functional>
#include <mutex>



using namespace std;
using namespace muduo;
using namespace muduo::net;
#include "json.hpp"
using json = nlohmann::json;

// 表示处理消息的事件回调方法类型
using MsgHander = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp time)>; 

// 聊天服务器业务类
class ChatService
{
private:
    ChatService();
    unordered_map<int,MsgHander> _msgHanderMap; // 消息id和其对应的业务处理方法 
    // 数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel; // 离线消息操作类对象
    FriendModel _friendModel; // 好友操作类对象
    GroupModel _groupModel; // 群组操作类对象

    // 互斥锁，保证_connMap的线程安全
    mutex _connMutex;
    
    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap; // key:用户id value:用户的通信连接

 

public:
    // 获取单例对象的接口函数
    static ChatService* instance();
    //获取消息对应的处理器
    MsgHander getHandler(int msgid);
    // 处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 注销业务
    void logout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    // 服务器异常，重置用户的状态信息
    void reset();


    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组聊天业务     
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
};


#endif