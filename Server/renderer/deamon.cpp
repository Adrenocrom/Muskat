#include "deamon.h"

Deamon::Deamon(int port, int num_threads) {
    this->port        = port;
    this->num_threads = num_threads;

    initServer();
}

void Deamon::initServer() {
    ptr_server = new WsServer(port, num_threads);
    auto& echo = ptr_server->endpoint["^/echo/?$"];

    echo.onmessage = [&](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::Message> message) {
        auto message_str=message->string();
        shared_ptr<WsServer::Connection>& con = connection;
        Deamon::sendMessage(con, message_str);

        cout << "Server: Message received: \"" << message_str << "\" from " << (size_t)connection.get() << endl;

        cout << "Server: Sending message \"" << message_str <<  "\" to " << (size_t)connection.get() << endl;


    };

    echo.onopen=[](shared_ptr<WsServer::Connection> connection) {
        cout << "Server: Opened connection " << (size_t)connection.get() << endl;
    };

    //See RFC 6455 7.4.1. for status codes
    echo.onclose=[](shared_ptr<WsServer::Connection> connection, int status, const string& reason) {
        cout << "Server: Closed connection " << (size_t)connection.get() << " with status code " << status << endl;
    };

    //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    echo.onerror=[](shared_ptr<WsServer::Connection> connection, const boost::system::error_code& ec) {
        cout << "Server: Error in connection " << (size_t)connection.get() << ". " <<
                "Error: " << ec << ", error message: " << ec.message() << endl;
    };

    server_thread = thread([&](){
        //Start WS-server
        ptr_server->start();

    });
}

void Deamon::sendMessage(shared_ptr<WsServer::Connection> connection, string message) {
    auto send_stream=make_shared<WsServer::SendStream>();
    *send_stream << "Hallo ich bin ein neuer toller string";

    //server.send is an asynchronous function
    ptr_server->send(connection, send_stream, [](const boost::system::error_code& ec){
        if(ec) {
            cout << "Server: Error sending message. " <<
            //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                    "Error: " << ec << ", error message: " << ec.message() << endl;
        }
    });
}

void Deamon::exitServer() {
    server_thread.join();
}

void Deamon::exit() {
    exitServer();
}
