//
// Created by lq on 2024/8/13.
//
#include "redis.h"
#include "Logging.h"

Redis::Redis() : publish_context_(nullptr), subscribe_context_(nullptr){}

Redis::~Redis(){
    if(publish_context_ != nullptr){
        redisFree(publish_context_);
    }
    if(subscribe_context_ != nullptr){
        redisFree(subscribe_context_);
    }
}

//连接
bool Redis::connet() {
    publish_context_ = redisConnect("127.0.0.1", 6379);
    if(publish_context_ == nullptr){
        LOG_FETAL << "redis connect failed!";
        return false;
    }
    subscribe_context_ = redisConnect("127.0.0.1", 6379);
    if(subscribe_context_ == nullptr){
        LOG_FETAL << "redis connect failed!";
        return false;
    }

    std::thread t([&](){observer_channel_message();});
    t.detach();

    LOG_INFO << "connect redis-server success!";
    return true;
}

bool Redis::publish(int channel, std::string message) {
    redisReply *reply = (redisReply*) redisCommand(publish_context_, "PUBLISH %d %s", channel, message.c_str());
    if(reply == nullptr){
        LOG_FETAL << "publish command failed!";
        return false;
    }
    freeReplyObject(reply);
    return true;
}

//向Redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel) {
    // redisCommand 会先把命令缓存到context中，然后调用RedisAppendCommand发送给redis
    // redis执行subscribe是族册，不会响应，不会返回reply
    // redis 127.0.0.1：6379
    if(REDIS_ERR == redisAppendCommand(subscribe_context_, "SUBSCRIBE %d", channel)){
        LOG_FETAL << "subscibe command failed";
        return false;
    }
    int done = 0;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(subscribe_context_, &done)){
            LOG_FETAL << "subscibe command failed";
            return false;
        }
    }
    return true;
}

bool Redis::unsubscribe(int channel) {
    if(REDIS_ERR == redisAppendCommand(subscribe_context_, "UNSUBSCRIBE %d", channel)){
        LOG_FETAL << "subscibe command failed";
        return false;
    }
    int done = 0;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(subscribe_context_, &done)){
            LOG_FETAL << "subscibe command failed";
            return false;
        }
    }
    return true;
}

//独立线程中接收订阅通道的消息
void Redis::observer_channel_message() {
    redisReply *reply = nullptr;
    while(REDIS_OK == redisGetReply(subscribe_context_, (void **)&reply)){
        if(reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr){
            notify_message_handler_(atoi(reply->element[1]->str), reply->element[2]->str);
        }

        freeReplyObject(reply);
    }
    LOG_FETAL << "oberver_channel_message quit";
}

void Redis::init_notify_handler(redis_handler handler) {
    notify_message_handler_ = handler;
}