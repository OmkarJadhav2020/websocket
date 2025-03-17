#include "websocket_client.h"
#include "cli_handler.h"
#include <third_party/CLI11/CLI11.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string server_url = "wss://echo.websocket.events/.ws";
    bool use_ssl = true;
    
    // Parse command line arguments
    CLI::App app{"WebSocket Client"};
    
    app.add_option("-s,--server", server_url, "WebSocket server URL")->required(false);
    app.add_flag("--ssl", use_ssl, "Use SSL/TLS (wss://)")->default_val(true);
    
    CLI11_PARSE(app, argc, argv);

    // Create WebSocket client
    WebSocketClient client(server_url, use_ssl);
    
    std::cout << "WebSocket Client" << std::endl;
    std::cout << "Server URL: " << server_url << std::endl;
    std::cout << "SSL: " << (use_ssl ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Type /help for available commands" << std::endl;

    // Create CLI handler
    CLIHandler cli_handler(client);
    
    // Connect to the server if specified
    if (client.connect()) {
        std::cout << "Connected to server" << std::endl;
    } else {
        std::cout << "Failed to connect automatically. Type /connect to try again." << std::endl;
    }
    
    // Run the CLI handler
    cli_handler.run();
    
    return 0;
}