#include "cli_handler.h"
#include <iostream>
#include <thread>
#include <chrono>

CLIHandler::CLIHandler(WebSocketClient& client)
    : client(client), running(false) {
    
    client.set_message_callback([this](const std::string& message) {
        handle_message(message);
    });
    
    client.set_connection_callback([this](bool connected) {
        handle_connection(connected);
    });
}

void CLIHandler::run() {
    running = true;
    
    while (running) {
        std::string input;
        std::getline(std::cin, input);
        
        if (input == "/quit" || input == "/exit") {
            running = false;
        } else if (input == "/help") {
            print_help();
        } else if (input == "/status") {
            std::cout << "Connection status: " << (client.is_connected() ? "Connected" : "Disconnected") << std::endl;
        } else if (input == "/connect") {
            if (!client.is_connected()) {
                std::cout << "Connecting..." << std::endl;
                if (client.connect()) {
                    std::cout << "Connected successfully" << std::endl;
                } else {
                    std::cout << "Failed to connect" << std::endl;
                }
            } else {
                std::cout << "Already connected" << std::endl;
            }
        } else if (input == "/disconnect") {
            if (client.is_connected()) {
                std::cout << "Disconnecting..." << std::endl;
                client.disconnect();
            } else {
                std::cout << "Not connected" << std::endl;
            }
        } else if (input.substr(0, 3) == "/b " && input.length() > 3) {
            if (client.is_connected()) {
                // Send as binary data (remove the "/b " prefix)
                client.send(input.substr(3), MessageType::BINARY);
            } else {
                std::cout << "Not connected. Type /connect to connect first" << std::endl;
            }
        } else if (!input.empty()) {
            if (client.is_connected()) {
                client.send(input);
            } else {
                std::cout << "Not connected. Type /connect to connect first" << std::endl;
            }
        }
        
        // Service the connection
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    if (client.is_connected()) {
        client.disconnect();
    }
}

void CLIHandler::handle_message(const std::string& message) {
    std::cout << "Received: " << message << std::endl;
}

void CLIHandler::handle_connection(bool connected) {
    if (connected) {
        std::cout << "Connected to server" << std::endl;
    } else {
        std::cout << "Disconnected from server" << std::endl;
    }
}

void CLIHandler::print_help() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  /help        - Show this help" << std::endl;
    std::cout << "  /connect     - Connect to the WebSocket server" << std::endl;
    std::cout << "  /disconnect  - Disconnect from the server" << std::endl;
    std::cout << "  /status      - Show connection status" << std::endl;
    std::cout << "  /quit, /exit - Exit the application" << std::endl;
    std::cout << "  /b <message> - Send a binary message to the server" << std::endl;
    std::cout << "  <message>    - Send a text message to the server" << std::endl;
}