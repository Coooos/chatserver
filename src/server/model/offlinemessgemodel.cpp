#include "offlinemessgemodel.hpp"
#include "db.h"

// 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into Offlinemessage(userid, message) values(%d, '%s')",
            userid, msg.c_str());

    // 2.连接数据库
    MySQL mysql;
    if (mysql.connect())
    {
        // 3.执行sql语句
        mysql.update(sql);
    }
}
// 删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from Offlinemessage where userid = %d", userid);

    // 2.连接数据库
    MySQL mysql;
    if (mysql.connect())
    {
        // 3.执行sql语句
        mysql.update(sql);
    }
}
// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from Offlinemessage where userid = %d", userid);

    vector<string> vec;
    // 2.连接数据库
    MySQL mysql;
    if (mysql.connect())
    {
        // 3.执行sql语句
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            // 4.遍历结果集合
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res); // 释放资源
        }
    }
    return vec; // 返回查询到的离线消息
}