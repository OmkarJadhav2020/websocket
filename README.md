# WebSocket Client

A C++ implementation of a WebSocket client with a command-line interface, built using the GN build system.

## Features

- Connect to WebSocket servers (both ws:// and wss:// protocols)
- Send and receive text messages
- Send and receive binary messages
- Command-line interface for user interaction
- Error handling and graceful disconnection
- Support for both GCC and Clang compilers
- Debug and release build configurations

## Prerequisites

To build and run this project, you need:

- GN (Generate Ninja) build system
- Ninja build tool
- C++ compiler (GCC or Clang)
- libwebsockets development files
- OpenSSL development files (for SSL/TLS support)

### Installing Dependencies on Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install build-essential ninja-build
sudo apt-get install libwebsockets-dev libssl-dev libz-dev
```

### Installing GN

Download the latest GN binary from the [Google repository](https://gn.googlesource.com/gn/) or build it from source.

## Building the Project

1. Generate the build files:

```bash
# For debug build (default)
gn gen out/Debug

# For release build
gn gen out/Release --args="is_debug=false"

# To use Clang instead of GCC
gn gen out/Debug --args="use_clang=true"
```

2. Build the project:

```bash
ninja -C out/Debug websocket_client
# or
ninja -C out/Release websocket_client
```

3. Run the tests:

```bash
ninja -C out/Debug websocket_client_test
./out/Debug/websocket_client_test
```

## Running the Application

```bash
# Connect to the default echo server (wss://echo.websocket.events/.ws)
./out/Debug/websocket_client

# Connect to a custom WebSocket server
./out/Debug/websocket_client --server wss://example.com/websocket

# Connect using ws:// protocol (non-secure)
./out/Debug/websocket_client --server ws://example.com/websocket --ssl=false
```

## Command-Line Interface

Once the application is running, the following commands are available:

- `/help` - Show available commands
- `/connect` - Connect to the WebSocket server
- `/disconnect` - Disconnect from the server
- `/status` - Show connection status
- `/quit` or `/exit` - Exit the application
- `/b <message>` - Send a binary message to the server
- `<message>` - Send a text message to the server

## Project Structure

- `src/` - Source code files
  - `main.cc` - Application entry point and command-line argument parsing
  - `websocket_client.h/cc` - WebSocket client implementation
  - `cli_handler.h/cc` - Command-line interface handler
- `tests/` - Unit tests
- `build/` - Build system configuration
- `third_party/` - Third-party libraries (CLI11)

## Implementation Details

### WebSocketClient

The `WebSocketClient` class uses libwebsockets to implement the WebSocket protocol. It supports:
- Connecting to WebSocket servers
- Sending text and binary messages
- Receiving messages
- Event callbacks for message reception and connection status

### CLIHandler

The `CLIHandler` class provides a simple command-line interface for interacting with the WebSocket client. It:
- Processes user commands
- Displays received messages
- Handles connection status updates

## License

This project is provided as-is for educational purposes.
