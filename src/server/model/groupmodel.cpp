#include "groupmodel.hpp"
#include "db.h"

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入成功的用户数据生成的主键id
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;// 创建群组成功
        }
    }
    return false;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d,%d,'%s')", groupid, userid, role.c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
// 查询用户所在的群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d", userid);     
    vector<Group> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            // 查出userid用户所属的所有群组信息
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                vec.push_back(group);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
// 查询群组用户id列表，除userid自己，主要用于群聊业务给群组其他成员发送消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);     
    vector<int> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            // 查出userid用户所属的所有群组信息
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return vec;
}