//
// Created by lq on 2024/7/21.
//

#ifndef CHAT_SERVER_CHATSERVER_H
#define CHAT_SERVER_CHATSERVER_H

#include "TcpServer.h"
#include "EventLoop.h"

class ChatServer{
public:
    // 初始化聊天服务器对象
    ChatServer(EventLoop *loop, const InetAddress & listenAddr,
               const std::string &nameArg);

    void start();
private:
    void onConnection(const TcpConnectionPtr &);
    void onMessage(const TcpConnectionPtr &,
                   Buffer *,
                   Timestamp);
    EventLoop *_loop;
    TcpServer _server;
};


#endif //CHAT_SERVER_CHATSERVER_H
