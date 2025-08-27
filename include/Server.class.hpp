#pragma once

#include "../include/Client.class.hpp"
#include "IrcFormatter.class.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <ifaddrs.h>
#include <iostream>
#include <map>
#include <poll.h>
#include <sstream>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <vector>

class	Channel;
class	Client;

class Server
{
  private:
	static bool _signal;
	long _port;
	std::string _password;
	int _socketFd;

	std::string _serverName;
	std::string _serverIp;
	std::string _serverVersion;

	std::vector<pollfd> _pollFds;

	std::vector<Channel *> _channels;
	std::vector<Client *> _clients;
	std::vector<Client *> _clientsToRemove;

	std::map<std::string, void (Server::*)(Client *,
		std::vector<std::string>)> _commandHandlers;

	void setupCommandHandlers();

	void acceptNewClient(void);
	void registerClient(Client *client);
	void receiveNewData(int fd);
	void logNewClient(Client* client);
	void logNewConnection(int fd);

	Client *getClient(int fd);
	Client *getClientByNickname(const std::string &nickname);
	void disconnectClient(int fd);
	
	bool nickIsValid(const std::string &newNick) const;
	bool nickIsUnique(Client *requestingClient, const std::string &newNick) const;

	void handleCommand(Client *client, std::string &line);
	void handleNick(Client *client, std::vector<std::string> args);
	void handlePass(Client *client, std::vector<std::string> args);
	void handleUser(Client *client, std::vector<std::string> args);
	void handleKick(Client *client, std::vector<std::string> args);
	void handleInvite(Client *client, std::vector<std::string> args);
	void handleTopic(Client *client, std::vector<std::string> args);
	void handleMode(Client *client, std::vector<std::string> args);
	void handlePrivmsg(Client *client, std::vector<std::string> args);
	void handleJoin(Client *client, std::vector<std::string> args);
	void handlePart(Client *client, std::vector<std::string> args);
	void handlePing(Client *client, std::vector<std::string> args);
	void handlePong(Client *client, std::vector<std::string> args);
	void handleQuit(Client *client, std::vector<std::string> args);
	
	Channel *getChannel(const std::string &channelName);
	Channel *createChannel(const std::string &channelName, const std::string &key, Client *client);
	void removeChannel(const std::string &channelName);
	void sendMessageToChannel(Client *sender, const std::string &target, const std::string &message);
	void sendMessageToUser(Client *sender, const std::string &target, const std::string &message);
	void printChannels(Client *client, std::vector<std::string> args);


	// Essai pour le bot
	void handleBot(Client *client, std::vector<std::string> args);
	void handleRockPaperScissors(Client *client, std::vector<std::string> args);

  public:
	Server(long port, const std::string &password);
	~Server();

	void init();
	void run();
	static void signalHandler(int signum);
};
