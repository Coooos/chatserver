#ifndef GROPPMODEL_HPP
#define GROPPMODEL_HPP

#include "group.hpp"
#include <vector>
#include <string>
using namespace std;

// 维护群组信息的操作接口方法
class GroupModel
{
public:
    // 创建群组         
    bool createGroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在的群组信息
    vector<Group> queryGroups(int userid);
    // 查询群组用户id列表，除userid自己，主要用于群聊业务给群组其他成员发送消息
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif