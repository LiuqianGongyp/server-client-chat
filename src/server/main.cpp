//
// Created by lq on 2024/8/13.
//
#include "ChatServer.h"
#include "ChatService.h"
#include "Logging.h"
#include <signal.h>

void resetHandler(int){
    LOG_INFO << "capture the SIGINT, will reset state\n";
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv){
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(atoi(argv[1]), "127.0.0.1");
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}