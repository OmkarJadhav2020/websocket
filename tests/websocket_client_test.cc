#include "../src/websocket_client.h"
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Simple test framework
#define TEST(name) void name()
#define ASSERT(condition) if (!(condition)) { std::cerr << "Assertion failed: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; exit(1); }

TEST(test_url_parsing) {
    WebSocketClient client("wss://echo.websocket.events/.ws");
    ASSERT(client.connect());
    client.disconnect();
    
    WebSocketClient client2("ws://echo.websocket.events/.ws", false);
    ASSERT(client2.connect());
    client2.disconnect();
}

TEST(test_connection) {
    WebSocketClient client("wss://echo.websocket.events/.ws");
    ASSERT(client.connect());
    ASSERT(client.is_connected());
    client.disconnect();
    ASSERT(!client.is_connected());
}

TEST(test_send_receive) {
    WebSocketClient client("wss://echo.websocket.events/.ws");
    std::string received_message;
    bool message_received = false;
    
    client.set_message_callback([&received_message, &message_received](const std::string& message) {
        received_message = message;
        message_received = true;
    });
    
    ASSERT(client.connect());
    ASSERT(client.is_connected());
    
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

int main() {
    std::cout << "Running tests..." << std::endl;
    
    test_url_parsing();
    test_connection();
    test_send_receive();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}