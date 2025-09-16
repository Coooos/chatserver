#ifndef GROUP_HPP
#define GROUP_HPP

#include "groupuser.hpp"
#include <string>
#include <vector>
using namespace std;

// 群组类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->_id = id;
        this->_name = name;
        this->_desc = desc;
    }
    void setId(int id)
    {
        this->_id = id;
    }
    void setName(string name)
    {
        this->_name = name;
    }
    void setDesc(string desc)
    {
        this->_desc = desc;
    }

    int getId() const
    {
        return _id;
    }
    string getName() const
    {
        return _name;
    }
    string getDesc() const
    {
        return _desc;
    }
    vector<GroupUser> getUsers() const
    {
        return _userVec;
    }

private:
    int _id;                    // 群组ID
    string _name;               // 群组名称
    string _desc;               // 群组描述
    vector<GroupUser> _userVec; // 群组用户列表
};

#endif