//
// Created by lq on 2024/8/13.
//

#ifndef CHAT_SERVER_GROUPMODEL_H
#define CHAT_SERVER_GROUPMODEL_H
#include "group.h"
#include "MysqlConnectionPool.h"
#include <string>
#include <vector>
class GroupModel{
public:
    //创建群组
    bool createGroup(MysqlConnectionPool* pool, Group &group);

    //加入群组
    void addGroup(MysqlConnectionPool* pool, int userid, int groupid, std::string role);

    //查询用户所在群组信息
    std::vector<Group> queryGroups(MysqlConnectionPool* pool, int userid);

    //根据指定的groupid查询用户id列表，除userid，主要用户群聊业务给群组其他成员群发消息
    std::vector<int> queryGroupUsers(MysqlConnectionPool* pool, int userid, int groupid);
};
#endif //CHAT_SERVER_GROUPMODEL_H
