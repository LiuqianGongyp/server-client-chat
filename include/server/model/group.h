//
// Created by lq on 2024/8/13.
//

#ifndef CHAT_SERVER_GROUP_H
#define CHAT_SERVER_GROUP_H
#include <string>
#include <vector>
#include "groupuser.h"
class Group{
public:
    Group(int id = -1, std::string name = "", std::string desc = "") :
    _id(id), _desc(desc), _name(name){}

    void setId(int id) { _id = id; }
    void setName(std::string name) { _name = name; }
    void setDesc(std::string desc) { _desc = desc; }

    int getId() { return _id; }
    std::string getName() { return _name; }
    std::string getDesc() { return _desc; }
    std::vector<GroupUser> &getUsers() { return _users; }
private:
    int _id;
    std::string _name;
    std::string _desc;
    std::vector<GroupUser> _users;
};

#endif //CHAT_SERVER_GROUP_H