//
// Created by lq on 2024/8/13.
//

#ifndef CHAT_SERVER_CHATSERVICE_H
#define CHAT_SERVER_CHATSERVICE_H
#include "TcpConnection.h"
#include "json.hpp"
#include "usermodel.h"
#include "offlinemessagemodel.h"
#include "friendmodel.h"
#include "groupmodel.h"
#include "redis.h"
#include "MysqlConnectionPool.h"

using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr&, json&, Timestamp)>;

//聊天服务器业务类
class ChatService{
public:
    //单例模式
    //线程安全
    static ChatService* instance() {
        static ChatService service;
        return &service;
    }

    //登录业务
    void loginHandler(const TcpConnectionPtr &conn, json & js, Timestamp time);
    //注册业务
    void registerHandler(const TcpConnectionPtr &conn, json & js, Timestamp time);
    //一对一聊天业务
    void oneChatHandler(const TcpConnectionPtr &conn, json& js, Timestamp time);
    //添加好友业务
    void addFriendHandler(const TcpConnectionPtr &conn, json& js, Timestamp time);
    //获取对应消息的处理器
    MsgHandler getHandler(int msgid);
    //创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json& js, Timestamp time);
    //加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json& js, Timestamp time);
    //群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json& js, Timestamp time);
    //处理客户端异常退出
    void clientCloseExceptionHandler(const TcpConnectionPtr &conn);
    //服务端异常终止之后的操作
    void reset();
    //redis订阅消息触发的回调函数
    void redis_subscribe_message_handler(int channel, std::string message);
private:
    ChatService();
    ChatService(const ChatService&) = delete;
    ChatService& operator=(const ChatService&) = delete;

    //存储消息id和其对应的业务处理方法
    std::unordered_map<int, MsgHandler> _msgHandlerMap;

    //存储在线用户的通信连接
    std::unordered_map<int, TcpConnectionPtr> _userConnMap;

    //定义互斥锁
    std::mutex _connMutex;

    //Redis操作对象
    Redis _redis;

    //mysql的数据连接池
    MysqlConnectionPool* _pool;

    //数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;
};

#endif //CHAT_SERVER_CHATSERVICE_H
