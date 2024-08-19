//
// Created by lq on 2024/7/20.
//

#ifndef CHAT_SERVER_USER_H
#define CHAT_SERVER_USER_H

#include <string>

class User{
public:
    User(int id = -1, std::string name = "",
         std::string password = "", std::string state = "offline")
         : _id(id),
         _name(name),
         _state(state)
         {}
    void setId(const int &id) { _id = id; }
    void setName(const std::string &name) { _name = name; }
    void setPassword(const std::string &password) { _password = password; }
    void setState(const std::string &state) { _state = state; }

    int getId() {return _id; }
    std::string getName() { return _name; }
    std::string getPassword() { return _password; }
    std::string getState() { return _state; }
private:
    int _id;
    std::string _name;
    std::string _password;
    std::string _state;
}

#endif //CHAT_SERVER_USER_H















