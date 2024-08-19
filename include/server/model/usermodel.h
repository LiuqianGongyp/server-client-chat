//
// Created by lq on 2024/7/20.
//

#ifndef CHAT_SERVER_USERMODEL_H
#define CHAT_SERVER_USERMODEL_H

#include "user.h"
#include "MysqlConnectionPool.h"

class UserModel{
public:
    //向User表中插入
    bool insert(MysqlConnectionPool* pool, User &user);

    //根据用户id在User表中查询
    User query(MysqlConnectionPool* pool, int id);

    //更新用户的状态信息
    bool updateState(MysqlConnectionPool* pool, User user);

    //重置用户状态信息
    void resetState(MysqlConnectionPool* pool);
};
#endif //CHAT_SERVER_USERMODEL_H
