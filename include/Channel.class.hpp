#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>

#include "../include/Client.class.hpp"

class Client;

class Channel {
	private:
		std::string _name;
		std::vector<Client*> _clients;
        std::vector<char> _modes;
        std::string _key;
        int _clientLimit;
        std::vector<Client*> _operators;
        std::vector<Client*> _invitedClients;
        std::string _topic;
        bool _hasTopic;
        std::set<Client*> _voicedClients;

	public:
        Channel(const std::string &name);
        ~Channel();

		const std::string &getName() const;

        void addClient(Client *client);
        void removeClient(Client *client);
        bool hasClient(Client *client) const;
        const std::vector<Client*> &getClients() const;

        void setMode(char mode);
        void unsetMode(char mode);
        bool hasMode(char mode) const;

        void setKey(const std::string &key);
        void unsetKey();
        bool checkKey(const std::string &key) const;
        bool hasKey() const;
        const std::string &getKey() const;

        void setClientLimit(int limit);
        void unsetClientLimit();
        int getClientLimit() const;
        bool isFull() const;
        bool isEmpty() const;

        void addOperator(Client *client);
        void removeOperator(Client *client);
        bool isOperator(Client *client) const;

        void inviteClient(Client *client);
        bool isInvited(Client *client) const;
        void removeInvitation(Client *client);

        void setTopic(const std::string &topic);
        const std::string &getTopic() const;
        bool hasTopic() const;
	    void broadcast(const std::string& message);
};
