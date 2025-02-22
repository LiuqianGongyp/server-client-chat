//
// Created by lq on 2024/6/20.
//

#ifndef MY_NETWORK_DEMO_INETADDRESS_H
#define MY_NETWORK_DEMO_INETADDRESS_H
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr) : addr_(addr){}

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in* getSockAddr() const { return &addr_; }
    void setSockAddr(const sockaddr_in &addr) {addr_ = addr; }
private:
    sockaddr_in addr_;
};


#endif //MY_NETWORK_DEMO_INETADDRESS_H
