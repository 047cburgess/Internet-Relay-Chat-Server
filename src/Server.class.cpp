#include "../include/Server.class.hpp"
#include <errno.h>
#include <cstring>

bool Server::_signal = false;

Server::Server(long port, const std::string &password)
	: _port(port), _password(password), _socketFd(-1), _serverName("ft_irc_server"), _serverVersion("1.0")
{
}


Server::~Server()
{
	// delete the channels
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete *it;
	_channels.clear();

	// delete the clients
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete *it;
	_clients.clear();

	// close the socket of the server
	if (_socketFd >= 0)
		close(_socketFd);

	_pollFds.clear();
    	_commandHandlers.clear();
}


void Server::init()
{
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd < 0)
	{
		std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);
	

	if (bind(_socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
		close(_socketFd);
		
		exit(EXIT_FAILURE);
	}

	if (listen(_socketFd, SOMAXCONN) < 0)
	{
		std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
		close(_socketFd);
		
		exit(EXIT_FAILURE);
	}

	struct pollfd serverPollfd;
	bzero(&serverPollfd, sizeof(pollfd));
	serverPollfd.fd = _socketFd;
	serverPollfd.events = POLLIN;
	_pollFds.push_back(serverPollfd);

	struct ifaddrs *ifaddr, *ifa;
	if (getifaddrs(&ifaddr) == -1)
	{
		std::cerr << "Error getting network interfaces: " << strerror(errno) << std::endl;
		close(_socketFd);
		exit(EXIT_FAILURE);
	}
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
		{
			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ip, sizeof(ip));
			if (strcmp(ifa->ifa_name, "lo") != 0)
			{ // Exclude loopback interface
				_serverIp = ip;
				break;
			}
		}
	}
	if (ifa == NULL)
	{
		std::cerr << "No suitable network interface found." << std::endl;
		freeifaddrs(ifaddr);
		close(_socketFd);
		exit(EXIT_FAILURE);
	}
	freeifaddrs(ifaddr);
	this->setupCommandHandlers();

	std::cout << "Server initialized and listening on port " << _serverIp << ":" << _port << std::endl;
}


void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl;
	_signal = true;
}


void Server::acceptNewClient()
{
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(_socketFd, (sockaddr *)&(cliadd), &len); //-> accept the new client
	if (incofd == -1)
	{
		std::cout << "accept() failed" << std::endl;
		return;
	}

	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
	{
		std::cout << "fcntl() failed" << std::endl;
		return;
	}

	NewPoll.fd = incofd;	 //-> add the client socket to the pollfd
	NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
	NewPoll.revents = 0;	 //-> set the revents to 0

	Client *cli = new Client(incofd, inet_ntoa((cliadd.sin_addr))); //-> create a new client

	_clients.push_back(cli);										//-> add the client to the vector of clients
	_pollFds.push_back(NewPoll);									//-> add the client socket to the pollfd

	std::cout << "Client <" << incofd << "> Connected" << std::endl;
}


void Server::run(void)
{
	while (_signal == false)
	{
		if ((poll(&_pollFds[0], _pollFds.size(), -1) == -1) && Server::_signal == false)
		{
			throw(std::runtime_error("poll() failed"));
		}

		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _socketFd)
					acceptNewClient();
				else
					receiveNewData(_pollFds[i].fd);
			}
		}
	}
}


void Server::receiveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	
	size_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);

	if (bytes <= 0)
		disconnectClient(fd);
	else
	{
		// PARSE THE DATA AND PROCESS IT
		buff[bytes] = '\0';
		std::cout << "Server received data from Client <" << fd << ">: " << buff << std::endl;

		Client *client = getClient(fd);
		if (!client)
			return ;

		// Extract and process complete lines
		std::string &buffer = client->getMessageBuffer();
		buffer += buff;
		size_t pos;
		while ((pos = buffer.find('\n')) != std::string::npos)
		{
			std::string line = buffer.substr(0, pos);
			buffer.erase(0, pos + 1); // Remove up to and including '\n'

			// Optionally strip trailing \r
			if (line.size() > 0 && line[line.size() - 1] == '\r')
    			line.erase(line.size() - 1, 1);

			handleCommand(client, line); // Pass full command to command handler
			if (!getClient(fd))
				return ;
		}
	}
}

// Splits the input by spaces, except when found ':' is keeps all the rest including spaces
// "USER" "casey" "casey" "casey" ":Casey Christina Burgess"
std::vector<std::string> split(std::string& line)
{
	std::istringstream iss(line);
	std::string buffer;
	std::vector<std::string> args;
	while (iss >> buffer)
	{
		if (buffer[0] == ':')
		{
			size_t pos = line.find(':');
			buffer = line.substr(pos);
			args.push_back(buffer);
			return (args);
		}
		args.push_back(buffer);
	}
	return (args);
}


void	Server::setupCommandHandlers()
{
	_commandHandlers["NICK"] = &Server::handleNick;
	_commandHandlers["PASS"] = &Server::handlePass;
	_commandHandlers["USER"] = &Server::handleUser;
	_commandHandlers["KICK"] = &Server::handleKick;
	_commandHandlers["INVITE"] = &Server::handleInvite;
	_commandHandlers["TOPIC"] = &Server::handleTopic;
	_commandHandlers["MODE"] = &Server::handleMode;
	_commandHandlers["PRIVMSG"] = &Server::handlePrivmsg;
	_commandHandlers["JOIN"] = &Server::handleJoin;
	_commandHandlers["PART"] = &Server::handlePart;
	_commandHandlers["PING"] = &Server::handlePing;
	_commandHandlers["PONG"] = &Server::handlePong;
	_commandHandlers["PRINTCHANNELS"] = &Server::printChannels;
	_commandHandlers["TIME"] = &Server::handleBot; // Test for bot
	_commandHandlers["RPS"] = &Server::handleRockPaperScissors;

}


void Server::handleCommand(Client *client, std::string &line)
{
	std::vector<std::string> args = split(line);
	if (args.empty())
		return ;
	std::string command = args[0];

	std::map<std::string, void (Server::*)(Client*, std::vector<std::string>)>::iterator it;
	it = _commandHandlers.find(command);
	if (it != _commandHandlers.end())
		(this->*(it->second))(client, args);
	else
	{
		std::string errorResponse = IrcMessageFormatter::unknownCommand(_serverName, command);
		send(client->getSocket(), errorResponse.c_str(), errorResponse.size(), 0);
	}
}


void	Server::registerClient(Client* client)
{
	std::string response;

	if (client->isRegistered())
		return;
	client->setRegistered(true);
	this->logNewClient(client);

	response = IrcMessageFormatter::welcome(_serverName, client->getNickname(), client->getRealname(), client->getHostname());
	send(client->getSocket(), response.c_str(), response.size(), 0);

	response = IrcMessageFormatter::yourHost(_serverName, client->getNickname(), _serverVersion);
	send(client->getSocket(), response.c_str(), response.size(), 0);

	response = IrcMessageFormatter::motdStart(_serverName, client->getNickname());
	send(client->getSocket(), response.c_str(), response.size(), 0);

	response = IrcMessageFormatter::motdLine(_serverName, client->getNickname(), "    Have a wonderful day!!   ");
	send(client->getSocket(), response.c_str(), response.size(), 0);

	response = IrcMessageFormatter::motdEnd(_serverName, client->getNickname());
	send(client->getSocket(), response.c_str(), response.size(), 0);

	std::cout << "Sent MOTD to the client: " << client->getNickname() << std::endl;
}

void Server::disconnectClient(int fd)
{
	// find the client to remove
	Client *client = getClient(fd);

	// remove client's pollfd from _pollFds
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollFds.erase(it);
			break;
		}
	}

	// remove client from server list
	_clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());

	std::string quitMsg = IrcMessageFormatter::quit(client->getNickname(), client->getUsername(),
	_serverIp, "Leaving");

	std::vector<Channel*> chanCopy = client->getChannelsList();
	for (size_t i = 0; i < chanCopy.size(); ++i)
	{
		Channel* current = chanCopy[i];
		// remove the client from its channel
		current->removeClient(client);
		// remove the channel from the client list
		client->leaveChannel(current);

		// if is empty delete it from server
		if (current->isEmpty())
		{
			_channels.erase(std::remove(_channels.begin(), _channels.end(), current), _channels.end());
			delete current;
		}
		else
			current->broadcast(quitMsg);
	}

	delete client;
}


Client *Server::getClient(int fd)
{
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i]->getSocket() == fd) {
			return _clients[i];
		}
	}
	return (NULL);
}


Client *Server::getClientByNickname(const std::string& nickname)
{
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i]->getNickname() == nickname) {
			return _clients[i];
		}
	}
	return (NULL);
}


Channel *Server::getChannel(const std::string &channelName)
{
	std::vector<Channel *>::iterator ite = _channels.end();
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != ite; ++it)
	{
		if ((*it)->getName() == channelName)
			return (*it);
	}
	return (NULL);
}


Channel *Server::createChannel(const std::string &channelName, const std::string &key, Client *client)
{
	Channel *newChannel = new Channel(channelName);
	newChannel->setKey(key);
	_channels.push_back(newChannel);
	newChannel->addClient(client);
	client->setOperator(true); // Automatically make the client an operator in the new channel
	newChannel->addOperator(client);

	std::cout << "Channel <" << channelName << "> created by " << client->getNickname() << std::endl;
	std::cout << client->getNickname() << " is now an operator in channel <" << channelName << ">" << std::endl;
	return newChannel;
}


void Server::removeChannel(const std::string &channelName)
{
	std::vector<Channel *>::iterator ite = _channels.end();
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != ite; ++it)
	{
		if ((*it)->getName() == channelName)
		{
			delete *it;
			it = _channels.erase(it);
			std::cout << "Server deleted Channel <" << channelName << "> removed." << std::endl;
			return;
		}
	}
}


void	Server::printChannels(Client *client, std::vector<std::string> args)
{
	(void)client;
	(void)args;
	std::vector<Channel*>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		std::cout << "CHANNEL NAME: " << (*it)->getName() << std::endl;
		std::vector<Client*> clients = (*it)->getClients();
		std::vector<Client*>::iterator itt = clients.begin();
		while (itt != clients.end())
		{
			std::cout << (*itt)->getNickname() << std::endl;
			itt++;
		}
		it++;
	}
}

void	Server::logNewClient(Client* client)
{
	std::cout << "------------------------------" << std::endl;
    	std::cout << "📢 New Client Registered!" << std::endl;
    	std::cout << "Nickname: " << client->getNickname() << std::endl;
    	std::cout << "Real Name: " << client->getRealname() << std::endl;
    	std::cout << "IP Address: " << client->getIp() << std::endl;
    	std::cout << "Socket: " << client->getSocket() << std::endl;
    	std::cout << "------------------------------" << std::endl;
}
