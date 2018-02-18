#include "WebSocketExample.h"

WebSocketExample::WebSocketExample()
{
    _tcpServer.AddConnectionEstablishedListener(
        [this](TcpSocket& socket)
        {
            return OnConnectionEstablished(socket);
        }
    );

    _tcpServer.AddConnectionClosedListener(
        [this](TcpSocket& socket)
        {
            return OnConnectionClosed(socket);
        }
    );

    _tcpServer.AddDataAvailableListener(
        [this](TcpSocket& socket)
        {
            return OnDataAvailable(socket);
        }
    );

    _websocketServer.set_message_handler(
        [this](websocketpp::connection_hdl hdl, websocket_server::message_ptr msg)
        {
            OnMessageReceived(hdl, msg);
        }
    );

    _websocketServer.clear_access_channels(websocketpp::log::alevel::all);
    _websocketServer.set_access_channels(websocketpp::log::alevel::connect);
    _websocketServer.set_access_channels(websocketpp::log::alevel::disconnect);
    _websocketServer.set_access_channels(websocketpp::log::alevel::app);
}

int WebSocketExample::Main()
{
    if (!_tcpServer.Listen(9009))
    {
        return -1;
    }

    try
    {
        while (true)
        {
            _tcpServer.Poll(1000);
        }
    }
    catch (websocketpp::exception const & e)
    {
        std::cout << e.what() << std::endl;
    }
}

bool WebSocketExample::OnConnectionEstablished(TcpSocket &socket)
{
    websocket_server::connection_ptr con = _websocketServer.get_connection();
    con->set_write_handler(
        [&socket](websocketpp::connection_hdl, char const *buf, size_t size)
        {
            socket.Write(buf, size, false);
            return websocketpp::lib::error_code();
        }
    );
    con->start();
    _websocketConnections[&socket] = con;
    return true;
}

bool WebSocketExample::OnDataAvailable(TcpSocket &socket)
{
    char buf[4096];
    ssize_t bytes_read = socket.Read(buf, sizeof(buf));
    if (bytes_read>0)
    {
        auto con = _websocketConnections[&socket];
        if (con->read_all(buf, static_cast<size_t>(bytes_read)) != static_cast<size_t>(bytes_read))
        {
            socket.Close();
        }
    }
    return true;
}

bool WebSocketExample::OnConnectionClosed(TcpSocket &socket)
{
    auto con = _websocketConnections[&socket];
    con->eof();
    return true;
}

void WebSocketExample::OnMessageReceived(websocketpp::connection_hdl hdl, websocket_server::message_ptr msg)
{
    auto& log = _websocketServer.get_alog();

    if (msg->get_opcode() == websocketpp::frame::opcode::text)
    {
        log.write(websocketpp::log::alevel::app, "Text Message Received: "+msg->get_payload());
    }
    else
    {
        log.write(websocketpp::log::alevel::app, "Binary Message Received: "+websocketpp::utility::to_hex(msg->get_payload()));
    }

    try
    {
        _websocketServer.send(hdl, msg->get_payload(), msg->get_opcode());
    }
    catch (const websocketpp::lib::error_code& e)
    {
        log.write(websocketpp::log::alevel::app, "Echo Failed: "+e.message());
    }
}
