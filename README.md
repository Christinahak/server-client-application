# server-client-application
A simple Linux based client/server socket application written implemented in C.
The server handles up to 5 clients to connect simultaneously and supports basic command handling, including execution of some linux commands.

Server Features
    Client Limit: Allows a maximum of 5 clients to connect simultaneously.
    Command Handler:
        disconnect: Closes the connection with the client.
        shell "COMMAND": Executes the specified command on the server and sends the output back to the client. Supports basic Linux commands.

Client Features
    Command-Line Interface: Provides commands to interact with the server.
    Supported Commands:
        connect <IP_ADDRESS> <PORT>: Connects to the server at the specified IP address and port.
        disconnect: Disconnects from the server.
        shell "COMMAND": Sends a shell command to the server and displays the response.

Usage

  1. compile the program running command "make" in the terminal
  2. start the server by running "./server <port_number>" where <port_number> is a 4-digit munber
  3. to start the client, open a new terminal than run "./client"

Commands 

  Connecting to server 
      type "connect <ip_address> <port_number>" in the client terminal
            Example: 
                Client> connect 127.0.0.1 3000

  Disconnecting from server
      .type "disconnect"

  Running a shell command
      .type " shell "COMMAND" "
        Example:
          Client> shell "ls"

