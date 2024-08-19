//
// Created by lq on 2024/7/20.
//
#include "usermodel.h"
#include "MysqlConnectionPool.h"
#include <iostream>

bool UserModel::insert(MysqlConnectionPool *pool, User &user)  {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into user(name, password, state) values('%s', '%s', '%s')",
             user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->update(sql)){
        user.setId(conn->getLastInsertid());
        return true;
    }
    return false;
}

User UserModel::query(MysqlConnectionPool *pool, int id) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select * from user where id = %d;", id);

    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->query(sql) && conn->next()){
        User user;
        user.setId(atoi(conn->value(0).c_str()));
        user.setName(conn->value(1));
        user.setPassword(conn->value(2));
        user.setState(conn->value(3));
        return user;
    }
    return User();
}

bool UserModel::updateState(MysqlConnectionPool *pool, User user) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "update user set state = '%s' where id=%d", user.getState().c_str(), user.getId());

    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->update(sql)){
        return true;
    }
    return false;
}

void UserModel::resetState(MysqlConnectionPool *pool) {
    char sql[1024] = "update user set state = 'offline' where state = 'online'";

    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    conn->update(sql);
}