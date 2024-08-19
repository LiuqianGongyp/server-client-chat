//
// Created by lq on 2024/8/13.
//

#ifndef CHAT_SERVER_FRIENDMODEL_H
#define CHAT_SERVER_FRIENDMODEL_H
#include "user.h"
#include "MysqlConnectionPool.h"
#include <vector>

//维护好友信息的接口方法
class FriendModel{
public:
    //添加好友关系
    void insert(MysqlConnectionPool* pool, int userid, int friendId);

    //返回用户好友列表
    std::vector<User> query(MysqlConnectionPool* pool, int userId);
};
#endif //CHAT_SERVER_FRIENDMODEL_H
