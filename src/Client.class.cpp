#include "../include/Client.class.hpp"

Client::Client(int socket, char* ipAddr)
    : _socket(socket),  _ipAddr(ipAddr), _messageBuffer(""), _registered(false), _sentPassword(false), _sentNickname(false), 
        _sentUsername(false), _isAway(false), _isOperator(false), _lastPongTime(0),
        _lastActivityTime(time(NULL)), pingReceived(false)
{
    _lastActivityTime = time(NULL);
	std::cout << "------------------------------" << std::endl;
    	std::cout << "📢 New Client Connected!" << std::endl;
    	std::cout << "Socket: " << socket << std::endl;
    	std::cout << "IP Address: " << ipAddr << std::endl;
    	std::cout << "------------------------------" << std::endl;
}

Client::Client() {}


Client::~Client()
{
	// Close the clients fd
    shutdown(_socket, SHUT_RDWR);
    close(_socket);

    _channels.clear();
	std::cout << "------------------------------" << std::endl;
    	std::cout << "📢 Client Disconnected!" << std::endl;
    	std::cout << "Nickname: " << _nickname << std::endl;
    	std::cout << "Real Name: " << _realname << std::endl;
    	std::cout << "IP Address: " << _ipAddr << std::endl;
    	std::cout << "Socket: " << _socket << std::endl;
    	std::cout << "------------------------------" << std::endl;
}


int Client::getSocket() const
{
    return _socket;
}

const std::string &	Client::getIp() const
{
	return _ipAddr;
}

bool Client::isRegistered() const
{
    return _registered;
}


bool Client::readyToRegister() const
{
	if (_registered)
		return (false);
	if (_sentPassword && _sentNickname && _sentUsername)
		return (true);
	return (false);
}


void Client::setRegistered(bool status)
{
    _registered = status;
}


/**
 * @return true if the client has sent the PASS command, false otherwise
 */
bool Client::hasSentPassword() const
{
    return _sentPassword;
}


/**
 * Set the sentPass status of the client
 */
void Client::setSentPassword(bool status)
{
    _sentPassword = status;
}


/**
 * @return the message buffer of the client
 */
std::string& Client::getMessageBuffer()
{ 
    return _messageBuffer; 
}


/**
 * @return true if the client has sent the NICK command, false otherwise
 */
bool Client::hasSentNickname() const
{
    return _sentNickname;
}


/**
 * Set the sentNick status of the client
 */
void Client::setSentNickname(bool status)
{
    _sentNickname = status;
}


/**
 * @return true if the client has sent the USER command, false otherwise
 */
bool Client::hasSentUsername() const
{
    return _sentUsername;
}


/**
 * Set the sentUser status of the client
 */
void Client::setSentUsername(bool status)
{
    _sentUsername = status;
}


/* ID DU CLIENT */
const std::string &Client::getNickname() const
{
    return _nickname;
}


void Client::setNickname(const std::string &nickname)
{
    _nickname = nickname;
}


const std::string &Client::getUsername() const
{
    return _username;
}


void Client::setUsername(const std::string &username)
{
    _username = username;
}


const std::string &Client::getHostname() const
{
    return _hostname;
}


void Client::setHostname(const std::string &hostname)
{
    _hostname = hostname;
}


const std::string &Client::getServername() const
{
    return _servername;
}


void Client::setServername(const std::string &servername)
{
    _servername = servername;
}


const std::string &Client::getRealname() const
{
    return _realname;
}


void Client::setRealname(const std::string &realname)
{
    _realname = realname;
}


void Client::joinChannel(Channel *channel)
{
    _channels.push_back(channel);
}


void Client::leaveChannel(Channel *channel)
{
     std::vector<Channel*>::iterator ite = _channels.end();
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != ite; ++it)
    {
        if ((*it)->getName() == channel->getName()) {
		std::cout << _nickname << " removing channel from its channel list: " << channel->getName() << std::endl;
            _channels.erase(it);
        	return;
	}
    }
}


bool Client::isInChannel(const std::string &channelName) const
{
    std::vector<Channel*>::const_iterator ite = _channels.end();
    for (std::vector<Channel*>::const_iterator it = _channels.begin(); it != ite; ++it)
    {
        if ((*it)->getName() == channelName)
            return true;
    }
    return false;
}


const std::vector<Channel*> &Client::getChannelsList() const
{
    return _channels;
}

/**
 * Set the time of the last PONG received from the client
 */
void Client::setLastPongTime(time_t time)
{
    _lastPongTime = time;
}


/**
 * @return the time of the last PONG received from the client
 */
time_t Client::getLastPongTime() const
{
    return _lastPongTime;
}


bool Client::isAway() const
{
    return _isAway;
}


void Client::setAway(bool status)
{
    _isAway = status;
}


bool Client::isOperator() const
{
    return _isOperator;
}


void Client::setOperator(bool status)
{
    _isOperator = status;
}

void Client::updateLastActivity()
{
    _lastActivityTime = time(NULL);
}


/**
 * @return true if the client has received a PING, false otherwise
 */
bool Client::isPingReceived() const
{
    return pingReceived;
}


time_t Client::getLastActivityTime() const
{
    return _lastActivityTime;
}


void Client::setPingReceived(bool status)
{
    pingReceived = status;
}
