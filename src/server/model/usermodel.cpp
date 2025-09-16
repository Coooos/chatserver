#include "usermodel.hpp"
#include "db.h"
#include <iostream>
using namespace std;

// User表的增加方法
bool UserModel::insert(User &User)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into User(name, password, state) values('%s', '%s', '%s')",
            User.getName().c_str(), User.getPassword().c_str(), User.getState().c_str());

    // 2.连接数据库
    MySQL mysql;
    if (mysql.connect())
    {
        // 3.执行sql语句
        if (mysql.update(sql))
        {
            // 4.获取插入成功的用户数据生成的主键id
            User.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// 根据用户号码查询用户信息
User UserModel::query(int id)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id = %d", id);

    // 2.连接数据库
    MySQL mysql;
    if (mysql.connect())
    {
        // 3.执行sql语句
        MYSQL_RES *res = mysql.query(sql);
    }
    return false;
}

// 更新用户的状态信息
bool UserModel::updateState(User user)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update User set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    // 2.连接数据库
    MySQL mysql; // 会析构自动释放资源
    if (mysql.connect())
    {
        // 3.执行sql语句
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update User set state = 'offline' where state = 'online'");
    // 2.连接数据库
    MySQL mysql; // 会析构自动释放资源
    if (mysql.connect())
    {
        // 3.执行sql语句
        mysql.update(sql);
    }
}