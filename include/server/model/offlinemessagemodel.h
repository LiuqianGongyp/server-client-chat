//
// Created by lq on 2024/8/13.
//

#ifndef CHAT_SERVER_OFIILINEMESSAGEMODEL_H
#define CHAT_SERVER_OFIILINEMESSAGEMODEL_H
#include "MysqlConnectionPool.h"
#include <string>
#include <vector>

class OfflineMsgModel{
public:
    //存储用户的离线消息
    void insert(MysqlConnectionPool* pool, int userId, std::string msg);

    //删除用户的离线消息
    void remove(MysqlConnectionPool* pool, int userId);

    //查询用户的离线消息
    std::vector<std::string> query(MysqlConnectionPool* pool, int userId);
};
#endif //CHAT_SERVER_OFIILINEMESSAGEMODEL_H
