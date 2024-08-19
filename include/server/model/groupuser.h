//
// Created by lq on 2024/8/13.
//
#include <string>
#include "user.h"
#ifndef CHAT_SERVER_GROUPUSER_H
#define CHAT_SERVER_GROUPUSER_H
class GroupUser{
public:
    GroupUser(const User& user) : _user(user) {}
    void setRole(const std::string& role) { _role = role; }
    std::string getRole() { return _role; }
    std::string getName() { return _user.getName(); }
    std::string getState() { return _user.getState(); }
    int getId() {return _user.getId(); }
private:
    User _user;
    std::string _role;
};

#endif //CHAT_SERVER_GROUPUSER_H
