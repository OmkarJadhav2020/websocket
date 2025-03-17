#include "../src/websocket_client.h"
#include "../src/cli_handler.h"
#include <third_party/CLI11/CLI11.hpp>
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Simple test macros
#define TEST(name) void name()
#define ASSERT(condition) if (!(condition)) { std::cerr << "Assertion failed: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; exit(1); }

// Function to wait for connection - similar to main.cc logic
bool try_connect(WebSocketClient& client, int max_retries = 5) {
    // Try to connect
    if (!client.connect()) {
        std::cout << "Initial connection attempt failed, retrying..." << std::endl;
        
        // Retry a few times
        for (int i = 0; i < max_retries; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (client.connect()) {
                std::cout << "Successfully connected on retry " << (i+1) << std::endl;
                return true;
            }
        }
        return false;
    }
    
    // Give some time for the connection to establish
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return client.is_connected();
}

// Test URL parsing
TEST(test_url_parsing) {
    WebSocketClient client("wss://echo.websocket.events/.ws");
    ASSERT(try_connect(client));
    client.disconnect();
    
    WebSocketClient client2("ws://echo.websocket.events/.ws", false);
    ASSERT(try_connect(client2));
    client2.disconnect();
}

// Test connection lifecycle
TEST(test_connection) {
    WebSocketClient client("wss://echo.websocket.events/.ws");
    ASSERT(try_connect(client));
    ASSERT(client.is_connected());
    client.disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Allow time for disconnection
    ASSERT(!client.is_connected());
}

// Test send/receive
TEST(test_send_receive) {
    WebSocketClient client("wss://echo.websocket.events/.ws");
    std::string received_message;
    bool message_received = false;
    
    client.set_message_callback([&received_message, &message_received](const std::string& message) {
        received_message = message;
        message_received = true;
    });
    
    ASSERT(try_connect(client));
    
    client.send("Hello, WebSocket!");
    
    // Wait for response
    int timeout = 50; // 5 seconds timeout
    while (!message_received && timeout > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout--;
    }
    
    ASSERT(message_received);
    ASSERT(received_message == "Hello, WebSocket!");
    
    client.disconnect();
}

// Test CLI argument parsing and behavior
TEST(test_cli_handler) {
    std::string server_url = "wss://echo.websocket.events/.ws";
    bool use_ssl = true;
    WebSocketClient client(server_url, use_ssl);
    CLIHandler cli_handler(client);
    
    // Connect similar to main.cc
    if (!try_connect(client)) {
        std::cout << "Failed to connect in CLI handler test" << std::endl;
        ASSERT(false); // Fail the test
    }
    
    // Test command handler - uncomment when ready to test
    /*
    // Simulate CLI commands
    cli_handler.handle_command("/send Hello CLI");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Give time for processing
    cli_handler.handle_command("/disconnect");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    ASSERT(!client.is_connected());
    */
    
    // For now, just test basic connection
    ASSERT(client.is_connected());
    client.disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT(!client.is_connected());
}

// Run tests
int main() {
    std::cout << "Running tests..." << std::endl;
    
    test_url_parsing();
    test_connection();
    test_send_receive();
    test_cli_handler();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
