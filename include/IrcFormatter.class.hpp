#pragma once

#include "IrcCodes.hpp"
#include <string>

class IrcMessageFormatter {
    public:
        // Limite de taille des messages IRC
        static const size_t MAX_MESSAGE_LENGTH = 510;

        // Messages d'erreur
        static std::string needMoreParams(const std::string& serverName, const std::string& command);
        static std::string genericError(const std::string& serverName, const std::string& errorCode, const std::string& details);
        static std::string noSuchChannel(const std::string& serverName, const std::string& requester, const std::string& channelName);
        static std::string channelOperatorRequired(const std::string& serverName, const std::string& nickname, const std::string& channelName);
        static std::string userNotInChannel(const std::string& serverName, const std::string& nickname, const std::string& channelName);
        static std::string unknownMode(const std::string& serverName, const std::string& nickname, char modeChar);
        static std::string notOnChannel(const std::string& serverName, const std::string& channelName);
        static std::string noSuchNick(const std::string& serverName, const std::string& requester, const std::string& targetNick);
        static std::string userAlreadyOnChannel(const std::string& serverName, const std::string& targetNick, const std::string& channelName);
        static std::string badChannelMask(const std::string& serverName, const std::string& channelName);
        static std::string inviteOnlyChannel(const std::string& serverName, const std::string &nick, const std::string& channelName);
        static std::string badChannelKey(const std::string& serverName, const std::string& channelName);
        static std::string channelIsFull(const std::string& serverName, const std::string& channelName);
        static std::string cannotSendToChannel(const std::string& serverName, const std::string& nickname, const std::string& target);
        
        // Erreurs de commandes
        static std::string invalidCapSubcommand(const std::string& serverName, const std::string& nick, const std::string& subCommand);
        static std::string notRegistered(const std::string& serverName);
        static std::string unknownCommand(const std::string& serverName, const std::string& command);
        static std::string alreadyRegistered(const std::string& serverName);
        static std::string passwordMismatch(const std::string& serverName);
        
        // Erreurs de nickname
        static std::string noNicknameGiven(const std::string& serverName);
        static std::string erroneousNickname(const std::string& serverName, const std::string& newNickname);
        static std::string nicknameInUse(const std::string& serverName, const std::string& current, const std::string& newNickname);
        static std::string erroneousUsername(const std::string& serverName, const std::string& username);
        
        // Messages de bienvenue et d'information
        static std::string welcome(const std::string& serverName, const std::string& nickname, const std::string& realName, const std::string& host);
        static std::string yourHost(const std::string& serverName, const std::string& nick, const std::string& serverVersion);
        static std::string serverCreated(const std::string& serverName, const std::string& nick, const std::string& creationDate);
        static std::string myInfo(const std::string& serverName, const std::string& nick, const std::string& version, 
            const std::string& userModes, const std::string& channelModes);
            
        // Messages MOTD
        static std::string motdStart(const std::string& serverName, const std::string& nick);
        static std::string motdLine(const std::string& serverName, const std::string& nick, const std::string& message);
        static std::string motdEnd(const std::string& serverName, const std::string& nick);
        static std::string modeChanged(const std::string& serverName, const std::string& channelName, const std::string& modeString);

        // Messages de canal
        static std::string join(const std::string& nickname, const std::string& realname, const std::string& serverIp, const std::string& channelName);
        static std::string part(const std::string& nickname, const std::string& username, const std::string& serverIp, const std::string& channelName);
        static std::string modeChange(const std::string& nickname, const std::string& channelName, const std::string& modeString);
        static std::string channelModeIs(const std::string& serverName, const std::string& nickname, const std::string& channelName, 
                                        const std::string& modes, const std::string& modeParams = std::string());
        
        // Messages de topic
        static std::string topicReply(const std::string& serverName, const std::string& nickname, const std::string& channelName, const std::string& topic);
        static std::string noTopicReply(const std::string& serverName, const std::string& nickname, const std::string& channelName);
        static std::string topicChange(const std::string& nickname, const std::string& channelName, const std::string& topic);
        
        // Messages d'invitation
       // static std::string invite(const std::string &serverName, const std::string& nickname, const std::string& targetNick, const std::string& channelName);
        static std::string invite(const std::string& nickname, const std::string& targetNick, const std::string& channelName); 
       static std::string inviting(const std::string& serverName, const std::string& nickname, const std::string& targetNick, const std::string& channelName);
        
        // Messages de kick
        static std::string kick(const std::string& nickname, const std::string& channelName, const std::string& targetNick, const std::string& comment);
        static std::string quit(const std::string& nickname, const std::string& username, const std::string& serverIp, const std::string& message);
        
        // Messages de nick
        //static std::string nickChange(const std::string& currentNickname, const std::string& newNickname);
        static std::string nickChange(const std::string& currentNickname, const std::string& username, const std::string &serverIp, const std::string& newNickname);
        
        // Messages de noms
        static std::string namesReply(const std::string& serverName, const std::string& nick, const std::string& channelName, const std::string& nickList);
        static std::string endOfNames(const std::string& serverName, const std::string& nick, const std::string& channelName);
        
        // Messages de capacités
        static std::string capabilityList(const std::string& serverName, const std::string& nick, const std::string& capabilities);

	// Messages SENDER -> TARGET
        static std::string sendMsg(const std::string& sender, const std::string& target, const std::string& message);

	// PING/PONG
        static std::string pong(const std::string& message);
	

    private:
        // Utilitaire pour tronquer et ajouter CRLF
        static std::string formatMessage(const std::string& message);
        
        // Constructeur privé pour empêcher l'instanciation
        IrcMessageFormatter();
};
