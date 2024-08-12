# Server-Client Application

A simple Linux-based client/server socket application implemented in C. The server handles up to 5 clients simultaneously and supports basic command handling, including execution of some Linux commands.

## Server Features
- **Client Limit**: Allows a maximum of 5 clients to connect simultaneously.
- **Command Handler**:
  - **`disconnect`**: Closes the connection with the client.
  - **`shell "COMMAND"`**: Executes the specified command on the server and sends the output back to the client. Supports basic Linux commands.

## Client Features
- **Command-Line Interface**: Provides commands to interact with the server.
- **Supported Commands**:
  - **`connect <IP_ADDRESS> <PORT>`**: Connects to the server at the specified IP address and port.
  - **`disconnect`**: Disconnects from the server.
  - **`shell "COMMAND"`**: Sends a shell command to the server and displays the response.

## Usage

1. **Compile the Program**: Run the command `make` in the terminal.
   ```bash
   make
2. **Start the Server**: Run the server with the command `./server <port_number>`, where `<port_number>` is a 4-digit number.
   ```bash
   ./server 3000
3. Start the Client: Open a new terminal and run the client with the command `./client`.
   ```bash
   ./client

## Commands
 - **Connecting to the Server**
 - Type `connect <IP_ADDRESS> <PORT>` in the client terminal.
 - ```bash
   connect 127.0.0.1 3000
 -
 - **Disconnecting from the Server**
 - Type `disconnect` in the client terminal.
 - ```bash
   disconnect
-
