//
// Created by lq on 2024/8/13.
//
#include "ChatService.h"
#include "public.h"
#include "Logging.h"
#include <string>
#include <vector>

ChatService::ChatService() {
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::loginHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({REGISTER_MSG, std::bind(&ChatService::registerHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChatHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriendHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});

    if(_redis.connet()){
        _redis.init_notify_handler(std::bind(&ChatService::redis_subscribe_message_handler, this, std::placeholders::_1, std::placeholders::_2));
    }

    _pool = MysqlConnectionPool::getConnectionPool();
}

void ChatService::redis_subscribe_message_handler(int channel, std::string message) {
    std::lock_guard<std::mutex> lock(_connMutex);
    auto it = _userConnMap.find(channel);
    if(it != _userConnMap.end()){
        it->second->send(message);
        return;
    }
    _offlineMsgModel.insert(_pool, channel, message);
}

MsgHandler ChatService::getHandler(int msgid) {
    auto it = _msgHandlerMap.find(msgid);
    if(it == _msgHandlerMap.end()){
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp){
            LOG_ERROR << "msgId:" << msgid << " can not find handler!";
        };
    }
    return _msgHandlerMap[msgid];
}

void ChatService::reset() {
    _userModel.resetState(_pool);//理解成把所有用户强行下线
}

void ChatService::clientCloseExceptionHandler(const TcpConnectionPtr &conn) {
    User user;
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        for(auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it){
            if(it->second == conn){
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    _redis.unsubscribe(user.getId());
    if(user.getId() != -1){
        user.setState("offline");
        _userModel.updateState(_pool, user);
    }
}

void ChatService::oneChatHandler(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int toId = js["toid"].get<int>();

    {
        std::lock_guard<std::mutex> lock(_connMutex);
        auto it = _userConnMap.find(toId);
        if(it != _userConnMap.end()){
            it->second->send(js.dump());
            return;
        }
    }

    User user = _userModel.query(_pool, toId);
    if(user.getState() == "online"){
        _redis.publish(toId, js.dump());
        return;
    }

    _offlineMsgModel.insert(_pool, toId, js.dump());
}

void ChatService::addFriendHandler(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js["id"].get<int>();
    int friendId = js["friendid"].get<int>();

    _friendModel.insert(_pool, userId, friendId);
}

void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    Group group(-1, name, desc);
    if(_groupModel.createGroup(_pool, group)){
        _groupModel.addGroup(_pool, userId, group.getId(), "creator");
    }
}

void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    _groupModel.addGroup(_pool, userId, groupId, "normal");
}

void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    std::vector<int> userIdVec = _groupModel.queryGroupUsers(_pool, userId, groupId);

    std::lock_guard<std::mutex> lock(_connMutex);
    for(int id : userIdVec){
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end()){
            it->second->send(js.dump());
        }else{
            User user = _userModel.query(_pool, id);
            if(user.getState() == "online"){
                _redis.publish(id, js.dump());
            }else{
                _offlineMsgModel.insert(_pool, id, js.dump());
            }
        }
    }
}

void ChatService::loginHandler(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_DEBUG << "do login service!";

    int id = js["id"].get<int>();
    std::string password = js["password"];

    User user = _userModel.query(_pool, id);
    if(user.getId() == id && user.getPassword() == password){
        if(user.getState() == "online"){
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "this account is using, input another!";
            conn->send(response.dump());
        }else{
            {
                std::lock_guard<std::mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            _redis.subscribe(id);

            user.setState("online");
            _userModel.updateState(_pool, user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            std::vector<std::string> vec = _offlineMsgModel.query(_pool, id);
            if(!vec.empty()){
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(_pool, id);
            }else{
                LOG_INFO << "no offline message!";
            }

            std::vector<User> userVec = _friendModel.query(_pool, id);
            if(!userVec.empty()){
                std::vector<std::string> vec;
                for(auto user : userVec){
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec.push_back(js.dump());
                }
                response["friends"] = vec;
            }

            std::vector<Group> groupVec = _groupModel.queryGroups(_pool, id);
            if(!groupVec.empty()){
                std::vector<std::string> vec;
                for(auto group : groupVec){
                    json js;
                    js["id"] = group.getId();
                    js["groupname"] = group.getName();
                    js["groupdesc"] = group.getDesc();
                    std::vector<std::string> vec2;
                    for(auto groupuser : group.getUsers()){
                        json usersjs;
                        usersjs["id"] = groupuser.getId();
                        usersjs["name"] = groupuser.getName();
                        usersjs["state"] = groupuser.getState();
                        usersjs["role"] = groupuser.getRole();
                        vec2.push_back(usersjs.dump());
                    }
                    js["users"] = vec2;
                    vec.push_back(js.dump());
                }
                response["groups"] = vec;
            }

            conn->send(response.dump());
        }
    }
}

void ChatService::registerHandler(const TcpConnectionPtr &conn, json &js, Timestamp time) {
    LOG_DEBUG << "do regidster service!";

    std::string name = js["name"];
    std::string password = js["password"];

    User user;
    user.setName(name);
    user.setPassword(password);
    bool state = _userModel.insert(_pool, user);
    if(state){
        json response;
        response["msgid"] = REGISTER_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }else{
        json reponse;
        reponse["msgid"] = REGISTER_MSG_ACK;
        reponse["errno"] = 1;
        conn->send(reponse.dump());
    }
}