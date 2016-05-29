#ifndef DEAMON_H
#define DEAMON_H

#pragma once

#include "muskat.h"
#include "server_ws.hpp"


typedef SimpleWeb::SocketServer<SimpleWeb::WS> WsServer;

using namespace std;

class Deamon {
    private:
        thread server_thread;
        WsServer* ptr_server;
        int port;
        int num_threads;

public:
        Deamon(int port = 12345, int num_threads = 6);
        ~Deamon() {exit();}

        void initServer();
        void exitServer();

        void sendMessage(shared_ptr<WsServer::Connection> connection, string message);

        void exit();
};

#endif
