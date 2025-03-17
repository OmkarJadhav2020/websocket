#include "websocket_client.h"
#include <iostream>
#include <cstring>

WebSocketClient *WebSocketClient::instance = nullptr;

WebSocketClient::WebSocketClient(const std::string &server_url, bool use_ssl)
    : context(nullptr), connection(nullptr), server_url(server_url),
      use_ssl(use_ssl), connected(false),pending_message_type(MessageType::TEXT)
{

    // Parse URL
    size_t protocol_end = server_url.find("://");
    if (protocol_end != std::string::npos)
    {
        std::string protocol = server_url.substr(0, protocol_end);
        use_ssl = (protocol == "wss" || protocol == "https");

        size_t host_start = protocol_end + 3;
        size_t path_start = server_url.find("/", host_start);

        if (path_start == std::string::npos)
        {
            server_host = server_url.substr(host_start);
            server_path = "/";
        }
        else
        {
            server_host = server_url.substr(host_start, path_start - host_start);
            server_path = server_url.substr(path_start);
        }

        size_t port_start = server_host.find(":");
        if (port_start != std::string::npos)
        {
            server_port = std::stoi(server_host.substr(port_start + 1));
            server_host = server_host.substr(0, port_start);
        }
        else
        {
            server_port = use_ssl ? 443 : 80;
        }
    }

    // Save instance for callback
    instance = this;
}

WebSocketClient::~WebSocketClient()
{
    disconnect();
}

bool WebSocketClient::connect()
{
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = new lws_protocols[2]{
        {"websocket-client",
         callback_function,
         0,
         4096,
         0,
         nullptr,
         0},
        {nullptr, nullptr, 0, 0, 0, nullptr, 0} // terminator
    };
    info.gid = -1;
    info.uid = -1;
    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    context = lws_create_context(&info);
    if (!context)
    {
        std::cerr << "Failed to create libwebsockets context" << std::endl;
        return false;
    }

    struct lws_client_connect_info connect_info;
    memset(&connect_info, 0, sizeof(connect_info));

    connect_info.context = context;
    connect_info.address = server_host.c_str();
    connect_info.port = server_port;
    connect_info.path = server_path.c_str();
    connect_info.host = connect_info.address;
    connect_info.origin = connect_info.address;
    connect_info.ssl_connection = use_ssl ? LCCSCF_USE_SSL : 0;
    connect_info.protocol = "websocket-client";
    connect_info.pwsi = &connection;

    connection = lws_client_connect_via_info(&connect_info);
    if (!connection)
    {
        std::cerr << "Failed to connect to server" << std::endl;
        lws_context_destroy(context);
        context = nullptr;
        return false;
    }

    // Service the connection until connected or error
    int timeout = 10; // 10 seconds timeout
    while (!connected && timeout > 0)
    {
        lws_service(context, 100);
        timeout--;
    }

    return connected;
}

bool WebSocketClient::send(const std::string &message, MessageType type)
{
    if (!connected)
    {
        return false;
    }

    pending_message = message;
    pending_message_type = type;
    lws_callback_on_writable(connection);
    lws_service(context, 0);
    return true;
}

void WebSocketClient::disconnect()
{
    if (connection)
    {
        lws_callback_on_writable(connection);
        lws_service(context, 0);
        connection = nullptr;
    }

    if (context)
    {
        lws_context_destroy(context);
        context = nullptr;
    }

    connected = false;
    if (connection_callback)
    {
        connection_callback(false);
    }
}

bool WebSocketClient::is_connected() const
{
    return connected;
}

void WebSocketClient::set_message_callback(std::function<void(const std::string &)> callback)
{
    message_callback = callback;
}

void WebSocketClient::set_connection_callback(std::function<void(bool)> callback)
{
    connection_callback = callback;
}

int WebSocketClient::callback_function(struct lws *wsi, enum lws_callback_reasons reason,
                                       void *user, void *in, size_t len)
{
    if (!instance)
    {
        return 0;
    }

    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        instance->connected = true;
        if (instance->connection_callback)
        {
            instance->connection_callback(true);
        }
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        if (instance->message_callback)
        {
            std::string message(static_cast<char *>(in), len);
            instance->message_callback(message);
        }
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
        if (!instance->pending_message.empty())
        {
            unsigned char buf[LWS_PRE + instance->pending_message.size()];
            memcpy(buf + LWS_PRE, instance->pending_message.c_str(), instance->pending_message.size());

            enum lws_write_protocol write_protocol =
                (instance->pending_message_type == MessageType::BINARY)
                    ? LWS_WRITE_BINARY
                    : LWS_WRITE_TEXT;

            lws_write(wsi, buf + LWS_PRE, instance->pending_message.size(), write_protocol);
            instance->pending_message.clear();
        }
        break;

    case LWS_CALLBACK_CLIENT_CLOSED:
        instance->connected = false;
        if (instance->connection_callback)
        {
            instance->connection_callback(false);
        }
        break;

    default:
        break;
    }

    return 0;
}