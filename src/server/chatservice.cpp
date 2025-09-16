#include "chatservice.hpp"
#include "public.hpp"
#include "user.hpp"
#include <muduo/base/Logging.h>
#include <vector>

using namespace std;
using namespace muduo;
// 获取单例对象的接口函数
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 私有构造函数
// 注册消息以及对应的Handler
ChatService::ChatService()
{
    // 
    _msgHanderMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHanderMap.insert({LOGOUT_MSG, std::bind(&ChatService::logout, this, _1, _2, _3)});
    _msgHanderMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHanderMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHanderMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    // 群组业务
    _msgHanderMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHanderMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHanderMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
}

// 获取消息对应的处理器
MsgHander ChatService::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的处理方法
    auto it = _msgHanderMap.find(msgid);
    if (it == _msgHanderMap.end())
    {
        // 返回一个默认的处理方法 空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }
    else
    {
        return _msgHanderMap[msgid];
    }
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>(); // 用户id 编译成整形
    string pwd = js["password"];

    User user = _userModel.query(id);
    if (user.getId() == id && user.getPassword() == pwd)
    {
        if (user.getState() == "online")
        {
            // 用户已经登录，不能重复登录
            json response;
            response["msg"] = LOGIN_MSG_ACK;
            response["errno"] = 2; // 2表示用户已经登录
            response["errmsg"] = "this account is using, input another";
            conn->send(response.dump()); // 发送json字符串
            return;
        }
        else
        {
            // 登录成功
            {
                // 线程安全操作
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn}); // 记录用户的连接信息
            }
            // 登录成功 更新用户状态信息 offline->online
            user.setState("online");
            _userModel.updateState(user); // 更新用户状态

            json response;
            response["msg"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec; // 存储离线消息
                // 读取该用户的离线消息后，把该用户的离线消息删除
                _offlineMsgModel.remove(id);
            }
            // 查询该用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty())
            {
                // friend list
                vector<string> vec2;
                for (User &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            // 发送json字符串
            conn->send(response.dump());
        }
    }
    else
    {
        // 登录失败
        json response;
        response["msg"] = LOGIN_MSG_ACK;
        response["errno"] = "1";
        response["errmsg"] = "id or password is invalid";
        conn->send(response.dump()); // 发送json字符串
    }
}

void ChatService::logout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);// 线程安全
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);// 从用户连接表删除用户
        }
    }
    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);// 更新用户状态
}

// 处理注册业务 name password
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPassword(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["errno"] = 0;
        response["msg"] = REG_MSG_ACK;
        response["id"] = user.getId();
        conn->send(response.dump()); // 发送json字符串
    }
    else
    {
        // 注册失败
        json response;
        response["errno"] = 1;
        response["msg"] = REG_MSG_ACK;
        conn->send(response.dump()); // 发送json字符串
    }
}

// 客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    // 从_userConnMap表删除用户的连接信息
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);  // 获取用户id
                _userConnMap.erase(it); // 删除用户连接
                break;
            }
        }
    }
    // 更新用户的状态信息 （用户有效）
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// 服务器异常，重置用户的状态信息
void ChatService::reset()
{
    // 把所有的online状态的用户，设置成offline
    _userModel.resetState();
}

// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();
    // 线程安全
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid在线,直接转发消息
            it->second->send(js.dump());
            return;
        }
    }
    // toid不在线，存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}
// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    _friendModel.insert(userid, friendid);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    // 存储群组成员信息
    _groupModel.addGroup(userid, groupid, "normal");
}
// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    // 查询群组用户id列表，除userid自己
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);
    // 线程安全
    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发消息
            it->second->send(js.dump());
        }
        else
        {
            // 存储离线消息
            _offlineMsgModel.insert(id, js.dump());
        }
    }
}