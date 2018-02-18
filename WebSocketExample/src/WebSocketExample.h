#pragma once

#include <map>
#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>
#include "TcpServer/TcpServer.h"

typedef websocketpp::server<websocketpp::config::core> websocket_server;

class WebSocketExample
{
    public:
        WebSocketExample();
        int Main();

    private:
        TcpServer _tcpServer;
        websocket_server _websocketServer;
        std::map<TcpSocket*,websocket_server::connection_ptr> _websocketConnections;

        bool OnConnectionEstablished(TcpSocket& socket);
        bool OnDataAvailable(TcpSocket& socket);
        bool OnConnectionClosed(TcpSocket& socket);

        void OnMessageReceived(websocketpp::connection_hdl hdl, websocket_server::message_ptr msg);

};
