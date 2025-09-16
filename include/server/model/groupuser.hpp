#ifndef GROUPUSER_HPP
#define GROUPUSER_HPP
#include "user.hpp"

// 群组用户类 多了一个role属性
class GroupUser : public User
{
public:
    void setRole(string role) { this->_role = role; }
    string getRole() const { return this->_role; }
    
private:
    string _role; // 群组用户角色：群主、管理员、成员
};

#endif