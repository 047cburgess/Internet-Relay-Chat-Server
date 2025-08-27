#include "../include/Channel.class.hpp"

Channel::Channel(const std::string &name) {
	_name = name;
	_clientLimit = -1;
	_hasTopic = false;
};


Channel::~Channel() {
	std::cout << "channel destructor called: " << _name << std::endl;
	_clients.clear();
	_operators.clear();
	_invitedClients.clear();
	_voicedClients.clear();
};


const std::string &Channel::getName() const {
	return _name;	
};


/* About clients */
void Channel::addClient(Client *client) {
	if (std::find(_clients.begin(), _clients.end(), client) == _clients.end()) {
		_clients.push_back(client);
	}
};


void Channel::removeClient(Client *client) {
	std::vector<Client *>::iterator ite = _clients.end();
	for (std::vector<Client *>::iterator it = _clients.begin(); it != ite; ++it)
	{
		if (*it == client) {
			_clients.erase(it);
			std::cout << "Channel removed Client " << client->getSocket() << " removed from channel client list: " << this->_name << std::endl;
			return;
		}
	}
	return ;
};


bool Channel::hasClient(Client *client) const {
	return std::find(_clients.begin(), _clients.end(), client) != _clients.end();
};


const std::vector<Client*> & Channel::getClients() const {
	return _clients;
};


/* About modes */
void Channel::setMode(char mode) {
	if (mode == 'o' || mode == 'v' || mode == 'i' || mode == 'k' || mode == 'l' || mode == 't') {
		if (std::find(_modes.begin(), _modes.end(), mode) == _modes.end()) {
			_modes.push_back(mode);
		}
	} else {
		std::cerr << "Invalid mode: " << mode << std::endl;
	}	
};


void Channel::unsetMode(char mode) {
	if (mode == 'o' || mode == 'v' || mode == 'i' || mode == 'k' || mode == 'l' || mode == 't') {
		std::vector<char>::iterator it = std::find(_modes.begin(), _modes.end(), mode);
		if (it != _modes.end()) {
			_modes.erase(it);
		}
	} else {
		std::cerr << "Invalid mode: " << mode << std::endl;
	}	
};


bool Channel::hasMode(char mode) const {
	return std::find(_modes.begin(), _modes.end(), mode) != _modes.end();
};


/* About key */
void Channel::setKey(const std::string &key) {
	if (key.empty()) {
		std::cerr << "Key cannot be empty." << std::endl;
		return;
	}
	if (key.length() > 64) {
		std::cerr << "Key too long, must be less than 64 characters." << std::endl;
		return;
	}
	_key = key;
};


void Channel::unsetKey() {
	_key.clear();
	std::vector<char>::iterator it = std::find(_modes.begin(), _modes.end(), 'k');
	if (it != _modes.end()) {
		_modes.erase(it);
	}
	std::cout << "Key for channel " << _name << " has been unset." << std::endl;
};


bool Channel::checkKey(const std::string &key) const {
	return _key == key;	
};


bool Channel::hasKey() const {
	return !_key.empty();
};


const std::string &	Channel::getKey() const {
	return _key;
};


/* About limits and status of the channel */
void Channel::setClientLimit(int limit) {
	if (limit < 0) {
		std::cerr << "Client limit must be positive." << std::endl;
		return;
	}
	_clientLimit = limit;
	if (limit > 0) {
		if (std::find(_modes.begin(), _modes.end(), 'l') == _modes.end()) {
			_modes.push_back('l');
		}
	} else {
		_modes.erase(std::remove(_modes.begin(), _modes.end(), 'l'), _modes.end());
	}
};


void Channel::unsetClientLimit() {
	_clientLimit = -1;
	_modes.erase(std::remove(_modes.begin(), _modes.end(), 'l'), _modes.end());
};


int Channel::getClientLimit() const {
	return _clientLimit;
};


bool Channel::isFull() const {
	return _clientLimit > 0 && _clients.size() >= static_cast<size_t>(_clientLimit);	
};


bool Channel::isEmpty() const {
	return _clients.empty();
};


/* About operator */
void Channel::addOperator(Client *client) {
	if (std::find(_operators.begin(), _operators.end(), client) == _operators.end()) {
		_operators.push_back(client);
		client->setOperator(true);
		std::cout << "Client " << client->getSocket() << " is now an operator in channel: " << this->_name << std::endl;
	}
};


void Channel::removeOperator(Client *client) {
	if (std::find(_operators.begin(), _operators.end(), client) != _operators.end()) {
		_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
		client->setOperator(false);
		std::cout << "Client " << client->getSocket() << " is no longer an operator in channel: " << this->_name << std::endl;
	}
};


bool Channel::isOperator(Client *client) const {
	return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
};


void Channel::inviteClient(Client *client) {
	std::vector<Client *>::iterator it = std::find(_invitedClients.begin(), _invitedClients.end(), client);
	if (it == _invitedClients.end()) {
		_invitedClients.push_back(client);
	
		std::cout << "Client " << client->getSocket() << " has been invited to channel: " << this->_name << std::endl;
	} else {
		std::cerr << "Client " << client->getSocket() << " is already invited to channel: " << this->_name << std::endl;
	}
};


bool Channel::isInvited(Client *client) const {
	std::vector<Client *>::const_iterator it = std::find(_invitedClients.begin(), _invitedClients.end(), client);
	return it != _invitedClients.end();
};


void Channel::removeInvitation(Client *client) {
	std::vector<Client *>::iterator it = std::find(_invitedClients.begin(), _invitedClients.end(), client);
	if (it != _invitedClients.end()) {
		_invitedClients.erase(it);
		std::cout << "Client " << client->getSocket() << " invitation removed from channel: " << this->_name << std::endl;
	} else {
		std::cerr << "Client " << client->getSocket() << " is not invited to channel: " << this->_name << std::endl;
	}
};


/* About topic */
void Channel::setTopic(const std::string &topic) {
	if (topic.empty()) {
		std::cerr << "Topic cannot be empty." << std::endl;
		return;
	}
	if (topic.length() > 512) {
		std::cerr << "Topic too long, must be less than 512 characters." << std::endl;
		return;
	}
	_topic = topic;
	_hasTopic = true;
};


const std::string &	Channel::getTopic() const {
	return _topic;
};


bool Channel::hasTopic() const {
	return _hasTopic;
};


void	Channel::broadcast(const std::string& message)
{
	std::vector<Client*>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		send((*it)->getSocket(), message.c_str(), message.size(), 0);
		it++;
	}
}
