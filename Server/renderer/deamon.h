#ifndef DEAMON_H
#define DEAMON_H

#pragma once

#include"server_ws.hpp"

typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;

using namespace std;

class Deamon {
    private:
        thread server_thread;
        WsServer* ptr_server;

public:
        Deamon(int port = 12345, int num_threads = 1);
        ~Deamon() {exit();}

        void exit();
};

#endif
