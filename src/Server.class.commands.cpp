#include "Server.class.hpp"
// --> link for descriptions of the commands http://dicoinformatique.chez.com/irc.htm
//[(status)] /connect localhost 6667 password

/**
 * 
 * @description: This function handles the PASS command, which is used to authenticate a client with the server.
 * The client must send the correct password to proceed with registration.
 * If the password is incorrect or not provided, the client is disconnected.
 * SYNTAX : PASS <password>
 */
void	Server::handlePass(Client *client, std::vector<std::string> args)
{
	// Check if they already set their password, they can't do it twice
	if (client->hasSentPassword())
	{
		std::string errorResponse = IrcMessageFormatter::alreadyRegistered(_serverName);
		send(client->getSocket(), errorResponse.c_str(), errorResponse.size(), 0);
		return ;
	}
	
	// Make sure there is at least 2 arguments
	if (args.size() < 2)
	{
		std::string errorResponse = IrcMessageFormatter::needMoreParams(_serverName, args[0]);
		send(client->getSocket(), errorResponse.c_str(), errorResponse.size(), 0);
		return ;
	}
	
	// Check the password is correct, otherwise disconnect the client
	if (args[1] != _password)
	{
		std::string errorResponse = IrcMessageFormatter::passwordMismatch(_serverName);
		send(client->getSocket(), errorResponse.c_str(), errorResponse.size(), 0);
		disconnectClient(client->getSocket());
		return ;
	}

	// Update the status of this client
	client->setSentPassword(true);
	std::cout << "Client on socket <" << client->getSocket() << "> has authenticated password" << std::endl;
}

/* NICK 

La commande /nick vous permet de changer de nickname. Bien évidemment, il ne faut pas que votres
nick soit pris. votres nick ne doit pas excéder 9 caractères (en tout cas pour les serveurs
respectant la spécification RFC1459. Certains serveurs de type ConferenceRoom accepte des 
nicks de 30 caractères). Sur certains réseaux, on peut enregistrer des nicks. Vous etes à 
ce moment en possession d'un mot de passe, qu'il faut envoyer au serveur via la commande /pass.
Evites de prendre un nick enregistré car vous pouvez vous faire déconnecté (Killed). Enfin,
Undernet n'autorise qu'un certain nombre de changement de nickname en 10 secondes, afin d'éviter
le flood par changement de nick.

Syntaxe : /nick (nick) 
Example : /nick MEPHISTO- */

bool Server::nickIsValid(const std::string& newNick) const
{
	std::string specialChars("[\\]^_{|}");

	if (newNick.size() < 1 || newNick.size() > 9)
		return (false);
	if (!isalpha(newNick[0]) && specialChars.find(newNick[0]) == std::string::npos)
		return (false);

	size_t i = 0;
	while (i < newNick.size())
	{
		if (!isalnum(newNick[i]) && specialChars.find(newNick[i]) == std::string::npos)
			return (false);
		i++;
	}
	return (true);
}


bool	Server::nickIsUnique(Client* requestingClient, const std::string& newNick) const
{
	std::vector<Client*>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (*it == requestingClient)
			continue ;
		if ((*it)->getNickname() == newNick)
			return (false);
	}
	return (true);
}


/**
 * @description: This function handles the NICK command, which is used to set or change the client's nickname.
 * It checks if the nickname is valid and unique, and if so, updates the client's nickname.
 * If the client is already registered, it notifies other clients of the nickname change.
 * SYNTAX : NICK <nickname>
 */
void	Server::handleNick(Client *client, std::vector<std::string> args)
{
	std::string response;

	if (!client->hasSentPassword())
	{
		std::string response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		disconnectClient(client->getSocket());
		return ;
	}

	if (args.size() < 2)
		response = IrcMessageFormatter::noNicknameGiven(_serverName);
	else if (nickIsValid(args[1]) == false)
		response = IrcMessageFormatter::erroneousNickname(_serverName, args[1]);
	else if (nickIsUnique(client, args[1]) == false)
		response = IrcMessageFormatter::nicknameInUse(_serverName, client->getNickname(), args[1]);
	else
	{
		std::string currentNick = client->getNickname();
		client->setNickname(args[1]);
		client->setSentNickname(true);
		std::cout << "Client on socket <" << client->getSocket() << "> has set nick name : " << args[1] << std::endl;

		if (client->isRegistered())
		{
			response = IrcMessageFormatter::nickChange(currentNick, client->getUsername(), client->getIp(), args[1]);
			std::vector<Channel*>chans = client->getChannelsList();
			for (std::vector<Channel*>::iterator it = chans.begin(); it != chans.end(); it++)
				(*it)->broadcast(response);
		}
		else if (client->readyToRegister())
			registerClient(client);
	}
}

/**
 * @description: This function handles the USER command, which is used to set the client's username and other identity information.
 * It checks if the username is valid and updates the client's information accordingly.
 * If the client is ready to register, it calls the registerClient function.
 * SYNTAX : USER <username> <hostname> <servername> :<realname>
 */
void	Server::handleUser(Client *client, std::vector<std::string> args)
{
	std::string response;

	if (!client->hasSentPassword())
	{
		std::string response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		disconnectClient(client->getSocket());
		return ;
	}

	if (client->hasSentUsername() == true)
	{
		response = IrcMessageFormatter::alreadyRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;

	}
	
	if (args.size() < 5)
	{
		response = IrcMessageFormatter::needMoreParams(_serverName, args[0]);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args[4][0] != ':')
	{
		response = IrcMessageFormatter::erroneousUsername(_serverName, args[4]);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}
	
	// Set the client fieds
	client->setUsername(args[1]);
	client->setHostname(args[2]);
	client->setServername(args[3]);

	std::string realName = args[4];
	realName.erase(0, 1);
	client->setRealname(realName);
	client->setSentUsername(true);
	std::cout << "Client <" << client->getSocket() << "> has set their user." << std::endl;
	if (client->readyToRegister())
		registerClient(client);
	
}


/**
 * @description: This function handles the KICK command, which is used to remove a client from a channel.
 * It checks if the client has the necessary permissions to kick another client and sends appropriate responses.
 * SYNTAX : KICK <channel> <nickname> [<comment>]
 */
void	Server::handleKick(Client *client, std::vector<std::string> args)
{
	std::string response;

	if (!client->isRegistered())
	{
		response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args.size() < 3) {
		response = IrcMessageFormatter::needMoreParams(_serverName, "KICK");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}		
	std::string channelName = args[1];
	Channel *channel = this->getChannel(channelName);
	if (channel == NULL) {
		response = IrcMessageFormatter::noSuchChannel(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (!client->isInChannel(channelName)) {
		response = IrcMessageFormatter::notOnChannel(_serverName, channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (!channel->isOperator(client)) {
		response = IrcMessageFormatter::channelOperatorRequired(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	std::string targetNick = args[2];
	Client *targetClient = NULL;
	for (std::vector<Client *>::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it)
	{
		if ((*it)->getNickname() == targetNick) {
			targetClient = *it;
			break;
		}
	}
	if (targetClient == NULL) {
		response = IrcMessageFormatter::noSuchNick(_serverName, client->getNickname(), targetNick);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (!channel->isOperator(client)) {
		response = IrcMessageFormatter::channelOperatorRequired(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	std::string reason;
	if (args.size() == 4)
		reason = args[3];
	else
		reason = "";

	response = IrcMessageFormatter::kick(client->getNickname(), channelName, targetNick, reason);
	channel->broadcast(response);
	channel->removeClient(targetClient);
	targetClient->leaveChannel(channel);
	channel->removeInvitation(targetClient);
	std::cout << targetClient->getNickname() << " has been kicked from channel <" << channelName << ">" << std::endl;

}


/**
 * 
 * @description: This function handles the INVITE command, which is used to invite a client to a channel.
 * It checks if the client has the necessary permissions to invite another client and sends appropriate responses.
 * SYNTAX : INVITE <nickname> <channel>
 */
void	Server::handleInvite(Client *client, std::vector<std::string> args)
{
	std::string response;

	if (!client->isRegistered())
	{
		response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args.size() < 3) {
		response = IrcMessageFormatter::needMoreParams(_serverName, "INVITE");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}	

	std::string targetNick = args[1];
	std::string channelName = args[2];

	Channel *channel = this->getChannel(channelName);
	if (channel == NULL) {
		response = IrcMessageFormatter::noSuchChannel(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (!client->isInChannel(channelName)) {
		response = IrcMessageFormatter::notOnChannel(_serverName, channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (channel->hasMode('i') && !channel->isOperator(client)) {
		response = IrcMessageFormatter::channelOperatorRequired(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	Client *targetClient = NULL;
	for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNickname() == targetNick) {
			targetClient = *it;
			break;
		}
	}
	if (targetClient == NULL) {
		response = IrcMessageFormatter::noSuchNick(_serverName, client->getNickname(), targetNick);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (targetClient->isInChannel(channelName)) {
		response = IrcMessageFormatter::userAlreadyOnChannel(_serverName, targetNick, channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

	channel->inviteClient(targetClient);
	// Inform channel of invite
	response = IrcMessageFormatter::inviting(_serverName, client->getNickname(), targetNick, channelName);
    channel->broadcast(response);

	// Inform client that they are invited
    response = IrcMessageFormatter::invite(client->getNickname(), targetNick, channelName);
    send(targetClient->getSocket(), response.c_str(), response.size(), 0);

}


/**
 * @description: This function handles the TOPIC command, which is used to set or retrieve the topic of a channel.
 * It checks if the client has the necessary permissions to change the topic and sends appropriate responses.
 * SYNTAX : TOPIC <channel> [<topic>]
 */
void	Server::handleTopic(Client *client, std::vector<std::string> args)
{
	std::string response;

	if (!client->isRegistered())
	{
		response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args.size() < 2) {
		response = IrcMessageFormatter::needMoreParams(_serverName, "TOPIC");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}	

	std::string channelName = args[1];
	Channel *channel = this->getChannel(channelName);
	if (channel == NULL) {
		response = IrcMessageFormatter::noSuchChannel(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (!client->isInChannel(channelName)) {
		response = IrcMessageFormatter::notOnChannel(_serverName, channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (args.size() == 2) {
		// Retrieve the topic of the channel
		 if (!channel->hasTopic() || channel->getTopic().empty()) {
            response = IrcMessageFormatter::noTopicReply(_serverName, client->getNickname(), channelName);
            send(client->getSocket(), response.c_str(), response.size(), 0);
        } else {
            response = IrcMessageFormatter::topicReply(_serverName, client->getNickname(), channelName, channel->getTopic());
            send(client->getSocket(), response.c_str(), response.size(), 0);
        }
        return;
	}
	if (channel->hasMode('t') && !channel->isOperator(client)) {
        response = IrcMessageFormatter::channelOperatorRequired(_serverName, client->getNickname(), channelName);
        send(client->getSocket(), response.c_str(), response.size(), 0);
        return;
    }
    
    // Reconstruct the topic from remaining arguments (handling spaces)
    std::string topic;
    for (size_t i = 2; i < args.size(); ++i) {
        if (i > 2) topic += " ";
        topic += args[i];
    }
    
    // Remove leading ':' if present
    if (!topic.empty() && topic[0] == ':') {
        topic = topic.substr(1);
    }
    
    channel->setTopic(topic);
    
    // Notify all clients in the channel about the new topic
    for (std::vector<Client *>::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it)
    {
        std::string notifyResponse = IrcMessageFormatter::topicChange(client->getNickname(), channelName, topic);
        send((*it)->getSocket(), notifyResponse.c_str(), notifyResponse.size(), 0);
    }
    
    std::cout << "Topic for channel <" << channelName << "> changed to: " << topic << std::endl;
}


/**
 * @description: This function handles the MODE command, which is used to set or unset modes for a channel.
 * It checks if the client has the necessary permissions to change the modes and sends appropriate responses.
 * SYNTAX : MODE <channel> <mode> [<parameters>]
 */
void	Server::handleMode(Client *client, std::vector<std::string> args)
{
	std::cout << "Entered Handle Mode function" << std::endl;
	std::string response;

	if (!client->isRegistered())
	{
		response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args.size() < 3) {
		response = IrcMessageFormatter::needMoreParams(_serverName, "MODE");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}	

	std::string channelName = args[1];
	Channel *channel = this->getChannel(channelName);
	std::cout << "Channel name: " << channelName << std::endl;
	if (channel == NULL) {
		response = IrcMessageFormatter::noSuchChannel(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	std::cout << "Client is in channel: " << client->isInChannel(channelName) << std::endl;
	if (!client->isInChannel(channelName)) {
		response = IrcMessageFormatter::notOnChannel(_serverName, channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

    if (!channel->isOperator(client)) {
		std::cout << "Checkpoint 1" << std::endl;
        response = IrcMessageFormatter::channelOperatorRequired(_serverName, client->getNickname(), channelName);
        send(client->getSocket(), response.c_str(), response.size(), 0);
        return;
    }

	std::string modeString = args[2];
	if (modeString.empty()) {
		std::cout << "Checkpoint 2" << std::endl;
		response = IrcMessageFormatter::needMoreParams(_serverName, "MODE");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	bool adding = true;
    std::string processedModes = "";
    
    // Process each character in the mode string
    for (size_t i = 0; i < modeString.size(); ++i) {
        char modeChar = modeString[i];
        if (modeChar == '+') {
            adding = true;
            processedModes += "+";
        } else if (modeChar == '-') {
            adding = false;
            processedModes += "-";
        }
            // Process the mode character
		else if (modeChar == 'i' || modeChar == 't') {
			// Simple modes without parameters
			if (adding) {
				channel->setMode(modeChar);
				processedModes += modeChar;
				std::cout << "Mode +" << modeChar << " added to channel <" << channelName << ">" << std::endl;
			} else {
				channel->unsetMode(modeChar);
				processedModes += modeChar;
				std::cout << "Mode -" << modeChar << " removed from channel <" << channelName << ">" << std::endl;
			}
		} else if (modeChar == 'k') {
			// Key mode requires parameter
			if (adding) {
				if (args.size() > 3) {
					channel->setKey(args[3]);
					channel->setMode('k');
					processedModes += modeChar;
					std::cout << "Key set for channel <" << channelName << ">" << std::endl;
				} else {
					response = IrcMessageFormatter::needMoreParams(_serverName, "MODE");
					send(client->getSocket(), response.c_str(), response.size(), 0);
					return;
				}
			} else {
				channel->unsetKey();
				processedModes += modeChar;
				std::cout << "Key removed from channel <" << channelName << ">" << std::endl;
			}
		} else if (modeChar == 'l') {
			// Limit mode
			if (adding) {
				if (args.size() > 3) {
					int limit = std::atoi(args[3].c_str());
					channel->setClientLimit(limit);
					processedModes += modeChar;
					std::cout << "Client limit set to " << limit << " for channel <" << channelName << ">" << std::endl;
				} else {
					response = IrcMessageFormatter::needMoreParams(_serverName, "MODE");
					send(client->getSocket(), response.c_str(), response.size(), 0);
					return;
				}
			} else {
				channel->unsetClientLimit();
				processedModes += modeChar;
				std::cout << "Client limit removed from channel <" << channelName << ">" << std::endl;
			}
		} else if (modeChar == 'o') {
			// Operator mode requires target nickname
			if (args.size() > 3) {
				std::string targetNick = args[3];
				Client *targetClient = NULL;
				
				// Find the target client
				for (std::vector<Client *>::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it)
				{
					if ((*it)->getNickname() == targetNick) {
						targetClient = *it;
						break;
					}
				}
				
				if (targetClient == NULL) {
					response = IrcMessageFormatter::noSuchNick(_serverName, client->getNickname(), targetNick);
					send(client->getSocket(), response.c_str(), response.size(), 0);
					return;
				}
				
				if (adding) {
					channel->setMode('o');
					channel->addOperator(targetClient);
					processedModes += modeChar;
					std::cout << "Operator status granted to " << targetNick << " in channel <" << channelName << ">" << std::endl;
				} else {
					channel->unsetMode(modeChar);
					channel->removeOperator(targetClient);
					processedModes += modeChar;
					std::cout << "Operator status removed from " << targetNick << " in channel <" << channelName << ">" << std::endl;
				}
			} else {
				response = IrcMessageFormatter::needMoreParams(_serverName, "MODE");
				send(client->getSocket(), response.c_str(), response.size(), 0);
				return;
			}
		} else {
			response = IrcMessageFormatter::unknownMode(_serverName, client->getNickname(), modeChar);
			send(client->getSocket(), response.c_str(), response.size(), 0);
			return;
		}

    }
    
    response = IrcMessageFormatter::modeChange(client->getNickname(), channelName, processedModes);
    
    // Notify all clients in the channel
    for (std::vector<Client *>::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it)
    {
        send((*it)->getSocket(), response.c_str(), response.size(), 0);
    }
    
    std::cout << "Mode " << processedModes << " set for channel <" << channelName << ">" << std::endl;
}


/**
 * @description: This function sends a private message to a specific user.
 * If the target user does not exist, it sends an error message to the sender.
 * SYNTAX : PRIVMSG <target_nickname> <message>
 */
void	Server::sendMessageToUser(Client* sender, const std::string& targetNick, const std::string& message)
{
	std::string response;

	Client* target = getClientByNickname(targetNick);
	if (!target)
	{
		response = IrcMessageFormatter::noSuchNick(_serverName, sender->getNickname(), targetNick);
		send(sender->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

	response = IrcMessageFormatter::sendMsg(sender->getNickname(), targetNick, message);
	send(target->getSocket(), response.c_str(), response.size(), 0);
}

/**
 * @description: This function sends a message to all clients in a specific channel.
 * If the channel does not exist or the sender is not in the channel, it sends an error message to the sender.
 * SYNTAX : PRIVMSG <target_channel> <message>
 */
void	Server::sendMessageToChannel(Client* sender, const std::string& targetChannel, const std::string& message)
{
	std::string response;

	Channel* target = getChannel(targetChannel);

	// Check the channel exists
	if (!target)
	{
		response = IrcMessageFormatter::noSuchNick(_serverName, sender->getNickname(), targetChannel);
		send(sender->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

	// Check the sender is in the channel
	if (!sender->isInChannel(targetChannel))
	{
		response = IrcMessageFormatter::notOnChannel(_serverName, targetChannel);
		send(sender->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

	// Send the message to everyone in the channel except from themselves
	response = IrcMessageFormatter::sendMsg(sender->getNickname(), targetChannel, message);

	const std::vector<Client*>& clients = target->getClients();
	std::vector<Client*>::const_iterator it = clients.begin();
	while (it != clients.end())
	{
		if ((*it)->getNickname() != sender->getNickname())
		{
			std::cout << "sending msg: " << message << " to " << (*it)->getNickname() << std::endl;
			send((*it)->getSocket(), response.c_str(), response.size(), 0);
		}
		it++;
	}
}


/**
 * @description: This function handles the PRIVMSG command, which is used to send private messages to users or channels.
 * It checks if the command syntax is correct and sends the message to the appropriate target.
 * SYNTAX : PRIVMSG <target> :<message>
 */
void	Server::handlePrivmsg(Client *client, std::vector<std::string> args)
{
	std::cout << "Entered Handle Privmsg function" << std::endl;
	std::string response;
	std::string target;
	std::string message;

	if (!client->isRegistered())
	{
		response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args.size() < 3)
	{
		response = IrcMessageFormatter::needMoreParams(_serverName, "PRIVMSG");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

	if (args.size() > 3)
	{
		// to modify to invalid syntax ->what error?
		response = IrcMessageFormatter::needMoreParams(_serverName, "PRIVMSG");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	target = args[1];
	message = args[2];
	if (message[0] == ':')
		message.erase(0, 1);
	if (target[0] == '#')
		sendMessageToChannel(client, target, message);
	else
		sendMessageToUser(client, target, message);
}


/**
 * @description: This function handles the JOIN command, which is used to join a channel.
 * It checks if the channel exists, if the client has the necessary permissions to join, and sends appropriate responses.
 * If the channel does not exist, it creates a new channel.
 * SYNTAX : JOIN <channel> [<key>]
 */
void	Server::handleJoin(Client *client, std::vector<std::string> args)
{
	std::cout << "Entered Handle Join function" << std::endl;
	std::string response;
	
	if (!client->isRegistered())
	{
		response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args.size() < 2) {
		response = IrcMessageFormatter::needMoreParams(_serverName, "JOIN");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}	

	std::string command = args[0];
	std::string channelName = args[1];
	std::string key = args.size() > 2 ? args[2] : ""; 

	std::cout << "key in handleJoin: " << key << std::endl;
	if (channelName[0] != '#') {
		response = IrcMessageFormatter::badChannelMask(_serverName, channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

	Channel *channel = this->getChannel(channelName);
	if (channel == NULL) {
		channel = this->createChannel(channelName, key, client);
		std::cout << "Channel <" << channelName << "> created." << std::endl;
	}
	else {
		if (client->isInChannel(channelName)) {
			response = IrcMessageFormatter::userAlreadyOnChannel(_serverName, client->getNickname(), channelName);
			send(client->getSocket(), response.c_str(), response.size(), 0);
			return;
		}
		if (channel->hasKey() && !channel->checkKey(key)) {
			response = IrcMessageFormatter::badChannelKey(_serverName, channelName);
			send(client->getSocket(), response.c_str(), response.size(), 0);
			return;
		}
		if (channel->isFull()) {
			response = IrcMessageFormatter::channelIsFull(_serverName, channelName);
			send(client->getSocket(), response.c_str(), response.size(), 0);
			return;
		}
		if (channel->hasMode('i') && !channel->isInvited(client)) {
			response = IrcMessageFormatter::inviteOnlyChannel(_serverName, client->getNickname(), channelName);
			send(client->getSocket(), response.c_str(), response.size(), 0);
			return;
		}
	}




	channel->addClient(client);
	client->joinChannel(channel);
	response = IrcMessageFormatter::join(client->getNickname(), client->getUsername(), _serverIp, channelName);
	channel->broadcast(response);
	std::cout << "Client <" << client->getSocket() << "> has joined channel <" << channelName << ">" << std::endl;


	std::ostringstream oss;

	std::vector<Client*> chanClients = channel->getClients();
	for (unsigned int i = 0; i < chanClients.size(); i++)
	{
		if (channel->isOperator(chanClients[i]))
			oss << "@";
		oss << chanClients[i]->getNickname() << " ";
	}
	std::string nickList = oss.str();

        response = IrcMessageFormatter::namesReply(_serverName, client->getNickname(), channelName, nickList);
	send(client->getSocket(), response.c_str(), response.size(), 0);
        response = IrcMessageFormatter::endOfNames(_serverName, client->getNickname(), channelName);
	send(client->getSocket(), response.c_str(), response.size(), 0);
}


/**
 * @description: This function handles the PART command, which is used to leave a channel.
 * It checks if the client is in the channel and sends appropriate responses.
 * If the channel becomes empty after the client leaves, it removes the channel.
 * SYNTAX : PART <channel>
 */
void	Server::handlePart(Client *client, std::vector<std::string> args)
{
	std::string response;
	if (!client->isRegistered())
	{
		response = IrcMessageFormatter::notRegistered(_serverName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return ;
	}

	if (args.size() == 1) {
		response = IrcMessageFormatter::needMoreParams(_serverName, "PART");
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}	

	std::string channelName = args[1];
	Channel *channel = this->getChannel(channelName);
	if (channel == NULL) {
		response = IrcMessageFormatter::notOnChannel(_serverName, channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}
	if (!client->isInChannel(channelName)) {
		response = IrcMessageFormatter::userNotInChannel(_serverName, client->getNickname(), channelName);
		send(client->getSocket(), response.c_str(), response.size(), 0);
		return;
	}

	response = IrcMessageFormatter::part(client->getNickname(), client->getUsername(), client->getServername(), channelName);
	channel->broadcast(response);
	client->leaveChannel(channel);
	channel->removeClient(client);
	std::cout << "Client <" << client->getSocket() << "> has left channel <" << channelName << ">" << std::endl;

	if (channel->isEmpty()) {
		this->removeChannel(channelName);
	}

}

void	Server::handleQuit(Client *client, std::vector<std::string> args)
{
	(void)args;

	std::cout << "Entered handleQuit -> disconnecting client " << client->getNickname() << std::endl;
	disconnectClient(client->getSocket());
}


void	Server::handlePing(Client *client, std::vector<std::string> args)
{
	if (args.size() != 2)
		return;
	std::string message = args[1];

	std::string response = IrcMessageFormatter::pong(message);
	send(client->getSocket(), response.c_str(), response.size(), 0);
	std::cout << "Sent pong to " << client->getNickname() << std::endl;
}


void	Server::handlePong(Client *client, std::vector<std::string> args)
{
	(void) args;
	std::cout << "Received a pong from " << client->getNickname() << std::endl;
}


