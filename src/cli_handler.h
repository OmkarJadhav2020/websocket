#ifndef CLI_HANDLER_H
#define CLI_HANDLER_H

#include <string>
#include "websocket_client.h"

class CLIHandler {
public:
    CLIHandler(WebSocketClient& client);
    void run();

private:
    WebSocketClient& client;
    bool running;
    
    void handle_message(const std::string& message);
    void handle_connection(bool connected);
    void print_help();
};

#endif // CLI_HANDLER_H