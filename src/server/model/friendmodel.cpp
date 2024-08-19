//
// Created by lq on 2024/8/13.
//
#include "friendmodel.h"
#include "MysqlConnectionPool.h"
void FriendModel::insert(MysqlConnectionPool *pool, int userid, int friendId) {
    //组织sql语句
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into friend values (%d, %d)", userid, friendId);
    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    conn->update(sql);
}

//返回用户好友列表
std::vector<User> FriendModel::query(MysqlConnectionPool *pool, int userId) {
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on b.friend = a.id where b.userid = %d", userId);
    std::vector<User> vec;
    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->query(sql) && conn->resultNotEmpty()){
        while(conn->next()){
            User user;
            user.setId(atoi(conn->value(0).c_str()));
            user.setName(conn->value(1));
            user.setState(conn->value(2));
            vec.push_back(user);
        }
        return vec;
    }
    return vec;
}