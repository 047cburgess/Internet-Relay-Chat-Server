## ft_irc
*This project has been created as part of the 42 curriculum by caburges, dangtran and utiberto.*

## Description

**ft_irc** is a fully functional Internet Relay Chat (IRC) server implementation written in C++98. This project provides a complete IRC server capable of handling multiple concurrent client connections, supporting standard IRC commands, and maintaining channel-based communication.

### Key Features

- **Multi-client Support**: Handles multiple concurrent client connections using non-blocking socket I/O
- **Channel Management**: Create, join, and manage IRC channels
- **User Authentication**: Secure connection with password authentication
- **Standard IRC Commands**: Implements essential IRC commands including:
  - Connection commands: `PASS`, `NICK`, `USER`
  - Channel operations: `JOIN`, `PART`, `KICK`, `MODE`, `TOPIC`, `INVITE`
  - Messaging: `PRIVMSG`
  - Server management: `PING`, `PONG`, `QUIT`
- **Bot Integration**: Built-in bot functionality for automated server interactions
- **RFC Compliance**: Follows IRC protocol standards for interoperability with standard IRC clients

## Instructions

### Prerequisites

- **C++ Compiler**: `c++` (with C++98 support)
- **Make**: GNU Make or compatible version
- **Standard C++ Library**: Standard template library (STL)
- **Network Capabilities**: System must support socket programming (Linux/Unix)

### Compilation

Clone the repository and compile using Make:

```bash
cd ft_irc
make
```

This will generate the `ircserv` executable binary.

### Execution

Run the IRC server with the following command:

```bash
./ircserv <port> <password>
```

**Parameters:**
- `<port>`: The port number where the server will listen (e.g., 6667 for standard IRC)
- `<password>`: The password required for client authentication

**Example:**
```bash
./ircserv 6667 password
```

### Connecting to the Server

#### Using irssi (IRC Client)

**Option 1 - Direct connection:**
```bash
irssi -c localhost -w <password> -n <nickname> -p <port>
```

**Option 2 - Interactive mode:**
```bash
irssi
/connect localhost <port> <password> <nickname>
```

#### Using netcat (nc)

```bash
nc localhost <port>
PASS <password>
NICK <nickname>
USER <username> localhost localhost :<real name>
```

### Command Examples

Below are examples of the implemented IRC commands:

#### Authentication & Connection
```
PASS mypassword              # Authenticate with server password
NICK mynickname             # Set or change your nickname
USER myuser localhost localhost :My Real Name  # Register user details
```

#### Channel Operations
```
JOIN #general               # Join a channel
PART #general               # Leave a channel

TOPIC #general :New Topic   # Set channel topic
TOPIC #general              # View channel topic

KICK #general user1         # Remove user from channel

MODE #general +i            # Set channel mode (invite-only)
MODE #general -i            # Unset channel mode
MODE #general +o user1      # Give user operator status

INVITE user1 #general       # Invite user to channel
```

#### Messaging
```
PRIVMSG #general :Hello everyone!   # Send message to channel
PRIVMSG user1 :Hello!               # Send private message to user
```

#### Server Management
```
PING                        # Keep connection alive
PONG                        # Response to PING
QUIT                        # Disconnect from server
QUIT :Goodbye!              # Disconnect with message
```

### Development Commands

- **Build and run**: `make run` (starts server on port 6667 with password "password")
- **Memory leak detection**: `make leaks` (runs with Valgrind)
- **Clean object files**: `make clean`
- **Full clean**: `make fclean`
- **Rebuild**: `make re`

### File Transfer (via irssi)

```
/set dcc_download_path ~/Downloads
/dcc send <nickname> <filepath>
/dcc get <sender_nickname> <filepath>
```

## Troubleshooting

- **Port Already in Use**: If the port is already bound, try a different port or wait a moment before restarting
- **Permission Denied**: Ensure you have execute permissions on the compiled binary
- **Connection Refused**: Verify the server is running and listening on the specified port and address
- **Command Not Found**: Run `make` first to compile the project
