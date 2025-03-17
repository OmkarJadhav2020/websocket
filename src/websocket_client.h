#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <string>
#include <functional>
#include <libwebsockets.h>
enum class MessageType {
    TEXT,
    BINARY
};

class WebSocketClient {
public:
    WebSocketClient(const std::string& server_url, bool use_ssl = false);
    ~WebSocketClient();
    bool connect();
    bool send(const std::string& message, MessageType type = MessageType::TEXT);
    void disconnect();
    bool is_connected() const;
    void set_message_callback(std::function<void(const std::string&)> callback);
    void set_connection_callback(std::function<void(bool)> callback);

private:
    struct lws_context* context;
    struct lws* connection;
    MessageType pending_message_type = MessageType::TEXT;
    std::string server_url;
    std::string server_path;
    std::string server_host;
    int server_port;
    bool use_ssl;
    bool connected;
    std::string pending_message;
    std::function<void(const std::string&)> message_callback;
    std::function<void(bool)> connection_callback;

    static int callback_function(struct lws* wsi, enum lws_callback_reasons reason, 
                                void* user, void* in, size_t len);
    static WebSocketClient* instance;
};

#endif // WEBSOCKET_CLIENT_H