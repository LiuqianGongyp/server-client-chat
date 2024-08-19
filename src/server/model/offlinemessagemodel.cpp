//
// Created by lq on 2024/8/13.
//
#include "offlinemessagemodel.h"
#include "MysqlConnection.h"
void OfflineMsgModel::insert(MysqlConnectionPool *pool, int userId, std::string msg) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into offlinemessage values(%d, '%s')", userId, msg.c_str());

    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    conn->update(sql);
}

void OfflineMsgModel::remove(MysqlConnectionPool *pool, int userId) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "delete from offlinemessage where userid=%d", userId);

    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    conn->update(sql);
}

std::vector<std::string> OfflineMsgModel::query(MysqlConnectionPool *pool, int userId) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select message from offlinemessage where userid = %d", userId);

    std::vector<std::string> vec;
    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->query(sql) && conn->resultNotEmpty()){
        while(conn->next()){
            vec.push_back(conn->value(0));
        }
        return vec;
    }
    return vec;
}