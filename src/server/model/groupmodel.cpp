//
// Created by lq on 2024/8/13.
//
#include "groupmodel.h"
#include "MysqlConnection.h"

bool GroupModel::createGroup(MysqlConnectionPool *pool, Group &group) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into allgroup(groupname, groupdesc) values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());

    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->update(sql)){
        group.setId(conn->getLastInsertid());
        return true;
    }
    return false;
}

void GroupModel::addGroup(MysqlConnectionPool *pool, int userid, int groupid, std::string role) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into groupuser values(%d, %d, '%s')", groupid, userid, role.c_str());

    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    conn->update(sql);
}

std::vector<Group> GroupModel::queryGroups(MysqlConnectionPool *pool, int userid) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select a.id, a.groupname, a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d", userid);
    std::vector<Group> groupvec;
    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->query(sql) && conn->resultNotEmpty()){
        while(conn->next()){
            Group group;
            group.setId(atoi(conn->value(0).c_str()));
            group.setName(conn->value(1));
            group.setDesc(conn->value(2));
            groupvec.push_back(group);
        }
    }
    for(Group &group : groupvec){
        snprintf(sql, sizeof(sql), "select a.id,a.name,a.state,b.grouprole from user a inner join groupuser b on b.userid = a.id where b.groupid=%d", group.getId());
        if(conn->query(sql) && conn->resultNotEmpty()){
            while(conn->next()){
                User user;
                user.setId(atoi(conn->value(0).c_str()));
                user.setName(conn->value(1));
                user.setState(conn->value(2));
                GroupUser groupUser(user);
                groupUser.setRole(conn->value(3));
                group.getUsers().push_back(groupUser);

            }
        }
    }
    return groupvec;
}

std::vector<int> GroupModel::queryGroupUsers(MysqlConnectionPool *pool, int userid, int groupid) {
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d ans userid != %d", groupid, userid);
    std::vector<int> idVec;
    std::shared_ptr<MysqlConnection> conn = pool->getConnection();
    if(conn->query(sql) && conn->resultNotEmpty()){
        while(conn->next()){
            idVec.push_back(atoi(conn->value(0).c_str()));
        }
    }
    return idVec;
}