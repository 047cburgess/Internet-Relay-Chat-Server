#include "IrcFormatter.class.hpp"
#include <sstream>

// fonction principale
std::string IrcMessageFormatter::formatMessage(const std::string& message) {
    if (message.length() > MAX_MESSAGE_LENGTH) {
        return message.substr(0, MAX_MESSAGE_LENGTH) + "\r\n";
    }
    return message + "\r\n";
}

// Messages d'erreur de paramètres
std::string IrcMessageFormatter::needMoreParams(const std::string& serverName, const std::string& command) {
    return formatMessage(":" + serverName + ERR_NEEDMOREPARAMS + command + " :Not enough parameters");
}

std::string IrcMessageFormatter::genericError(const std::string& serverName, const std::string& errorCode, const std::string& details) {
    return formatMessage(":" + serverName + " " + errorCode + " " + details);
}

// Erreurs de canal
std::string IrcMessageFormatter::noSuchChannel(const std::string& serverName, const std::string& requester, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_NOSUCHCHANNEL + requester + " " + channelName + " :No such channel");
}

std::string IrcMessageFormatter::channelOperatorRequired(const std::string& serverName, const std::string& nickname, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_CHANOPRIVSNEEDED + nickname + " " + channelName + " :You're not channel operator");
}

std::string IrcMessageFormatter::userNotInChannel(const std::string& serverName, const std::string& nickname, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_USERNOTINCHANNEL + nickname + " " + channelName + " :They aren't on that channel");
}

std::string IrcMessageFormatter::unknownMode(const std::string& serverName, const std::string& nickname, char modeChar) {
    return formatMessage(":" + serverName + ERR_UNKNOWNMODE + nickname + " " + modeChar + " :is unknown mode char to me");
}

std::string IrcMessageFormatter::notOnChannel(const std::string& serverName, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_NOTONCHANNEL + channelName + " :You're not on that channel");
}

std::string IrcMessageFormatter::noSuchNick(const std::string& serverName, const::std::string& requester, const std::string& targetNick) {
    return formatMessage(":" + serverName + ERR_NOSUCHNICK + requester + " " + targetNick + " :No such nick/channel");
}

std::string IrcMessageFormatter::userAlreadyOnChannel(const std::string& serverName, const std::string& targetNick, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_USERONCHANNEL + targetNick + " " + channelName + " :is already on channel");
}

std::string IrcMessageFormatter::badChannelMask(const std::string& serverName, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_BADCHANMASK + channelName + " :Bad Channel Mask");
}

std::string IrcMessageFormatter::inviteOnlyChannel(const std::string& serverName, const std::string& nick, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_INVITEONLYCHAN + nick + " " + channelName + " :Cannot join channel (+i)");
}

std::string IrcMessageFormatter::badChannelKey(const std::string& serverName, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_BADCHANNELKEY + channelName + " :Cannot join channel (+k)");
}

std::string IrcMessageFormatter::channelIsFull(const std::string& serverName, const std::string& channelName) {
    return formatMessage(":" + serverName + ERR_CHANNELISFULL + channelName + " :Cannot join channel (+l)");
}

std::string IrcMessageFormatter::cannotSendToChannel(const std::string& serverName, const std::string& nickname, const std::string& target) {
    return formatMessage(":" + serverName + ERR_CANNOTSENDTOCHAN + nickname + " " + target + " :Cannot send to channel");
}

std::string IrcMessageFormatter::modeChanged(const std::string& serverName, const std::string& channelName, const std::string& modeString) {
    return formatMessage(":" + serverName + RPL_CHANNELMODEIS + channelName + " " + modeString);
}

// Erreurs de commandes
std::string IrcMessageFormatter::invalidCapSubcommand(const std::string& serverName, const std::string& nick, const std::string& subCommand) {
    return formatMessage(":" + serverName + ERR_INVALIDCAPCMD + nick + " " + subCommand + " :Invalid CAP subcommand");
}

std::string IrcMessageFormatter::notRegistered(const std::string& serverName) {
    return formatMessage(":" + serverName + ERR_NOTREGISTERED + ":You have not registered");
}

std::string IrcMessageFormatter::unknownCommand(const std::string& serverName, const std::string& command) {
    return formatMessage(":" + serverName + ERR_UNKNOWNCOMMAND + command + " :Unknown command");
}

std::string IrcMessageFormatter::alreadyRegistered(const std::string& serverName) {
    return formatMessage(":" + serverName + ERR_ALREADYREGISTRED + ":You may not reregister");
}

std::string IrcMessageFormatter::passwordMismatch(const std::string& serverName) {
    return formatMessage(":" + serverName + ERR_PASSWDMISMATCH + ":Password incorrect");
}

// Erreurs de nickname
std::string IrcMessageFormatter::noNicknameGiven(const std::string& serverName) {
    return formatMessage(":" + serverName + ERR_NONICKNAMEGIVEN + ":No nickname given");
}

std::string IrcMessageFormatter::erroneousNickname(const std::string& serverName, const std::string& newNickname) {
    return formatMessage(":" + serverName + ERR_ERRONEUSNICKNAME + newNickname + " :Erroneous nickname");
}

std::string IrcMessageFormatter::nicknameInUse(const std::string& serverName, const std::string& current, const std::string& newNickname) {
    return formatMessage(":" + serverName + ERR_NICKNAMEINUSE + current + " " + newNickname + " :Nickname is already in use");
}

std::string IrcMessageFormatter::erroneousUsername(const std::string& serverName, const std::string& username) {
    return formatMessage(":" + serverName + ERR_ERRONEUSUSERNAME + username + " :Erroneous username");
}

// Messages de bienvenue et d'information
std::string IrcMessageFormatter::welcome(const std::string& serverName, const std::string& nickname, const std::string& realName, const std::string& host) {
    return formatMessage(":" + serverName + RPL_WELCOME + nickname 
        + " :Welcome to the Internet Relay Network "
        + "Your nickname is " + nickname + " but your name is " + realName + "@" + host);
}

std::string IrcMessageFormatter::yourHost(const std::string& serverName, const std::string& nick, const std::string& serverVersion) {
    return formatMessage(":" + serverName + RPL_YOURHOST + nick 
        + " :Your host is " + serverName + ", running version " + serverVersion);
}

std::string IrcMessageFormatter::serverCreated(const std::string& serverName, const std::string& nick, const std::string& creationDate) {
    return formatMessage(":" + serverName + RPL_CREATED + nick 
        + " :This server was created " + creationDate);
}

std::string IrcMessageFormatter::myInfo(const std::string& serverName, const std::string& nick, const std::string& version, 
                                    const std::string& userModes, const std::string& channelModes) {
    return formatMessage(":" + serverName + RPL_MYINFO + nick + " " + serverName + " " + version 
        + " " + userModes + " " + channelModes);
}

// Messages MOTD
std::string IrcMessageFormatter::motdStart(const std::string& serverName, const std::string& nick) {
    return formatMessage(":" + serverName + RPL_MOTDSTART + nick + " :- " + serverName + " Message of the Day -");
}

std::string IrcMessageFormatter::motdLine(const std::string& serverName, const std::string& nick, const std::string& message) {
    return formatMessage(":" + serverName + RPL_MOTD + nick + " :- " + message);
}

std::string IrcMessageFormatter::motdEnd(const std::string& serverName, const std::string& nick) {
    return formatMessage(":" + serverName + RPL_ENDOFMOTD + nick + " :End of /MOTD command.");
}

// Messages de canal
std::string IrcMessageFormatter::join(const std::string& nickname, const std::string& realname, const std::string& serverIp, const std::string& channelName) {
    return formatMessage(":" + nickname + "!" + realname + "@" + serverIp + " JOIN :" + channelName);
}

std::string IrcMessageFormatter::part(const std::string& nickname, const std::string& username, const std::string& serverIp, const std::string& channelName) {
    return formatMessage(":" + nickname + "!" + username + "@" + serverIp + " PART " + channelName);
}

std::string IrcMessageFormatter::modeChange(const std::string& nickname, const std::string& channelName, const std::string& modeString) {
    return formatMessage(":" + nickname + " MODE " + channelName + " " + modeString);
}

std::string IrcMessageFormatter::channelModeIs(const std::string& serverName, const std::string& nickname, const std::string& channelName, 
                                           const std::string& modes, const std::string& modeParams) {
    std::string message = ":" + serverName + RPL_CHANNELMODEIS + nickname + " " + channelName + " " + modes;
    if (!modeParams.empty()) {
        message += " " + modeParams;
    }
    return formatMessage(message);
}

// Messages de topic
std::string IrcMessageFormatter::topicReply(const std::string& serverName, const std::string& nickname, const std::string& channelName, const std::string& topic) {
    return formatMessage(":" + serverName + RPL_TOPIC + nickname + " " + channelName + " :" + topic);
}

std::string IrcMessageFormatter::noTopicReply(const std::string& serverName, const std::string& nickname, const std::string& channelName) {
    return formatMessage(":" + serverName + RPL_NOTOPIC + nickname + " " + channelName + " :No topic is set");
}

std::string IrcMessageFormatter::topicChange(const std::string& nickname, const std::string& channelName, const std::string& topic) {
    return formatMessage(":" + nickname + " TOPIC " + channelName + " :" + topic);
}

// Messages d'invitation
// std::string IrcMessageFormatter::invite(const std::string& serverName, const std::string& nickname, const std::string& targetNick, const std::string& channelName) {
//     return formatMessage(":" + serverName + " NOTICE " + "@" + channelName + " :" + nickname + " invited " + targetNick + " into channel " + channelName);
// }

// std::string IrcMessageFormatter::inviting(const std::string& serverName, const std::string& nickname, const std::string& targetNick, const std::string& channelName) {
//     return formatMessage(":" + serverName + RPL_INVITING + nickname + " " + targetNick + " :" + channelName);
// }
std::string IrcMessageFormatter::invite(const std::string& nickname, const std::string& targetNick, const std::string& channelName) {
    return formatMessage(":" + nickname + " INVITE " + targetNick + " :" + channelName);
}

std::string IrcMessageFormatter::inviting(const std::string& serverName, const std::string& nickname, const std::string& targetNick, const std::string& channelName) {
    return formatMessage(":" + serverName + RPL_INVITING + nickname + " " + targetNick + " :" + channelName);
}



// Messages de kick
std::string IrcMessageFormatter::kick(const std::string& nickname, const std::string& channelName, const std::string& targetNick, const std::string& comment) {
    return formatMessage(":" + nickname + " KICK " + channelName + " " + targetNick + " :" + comment);
}

std::string IrcMessageFormatter::quit(const std::string& nickname, const std::string& username, const std::string& serverIp, const std::string& message) {
    return formatMessage(":" + nickname + "!" + username + "@" + serverIp + " QUIT " + message);
}

// Messages de nick
//std::string IrcMessageFormatter::nickChange(const std::string& currentNickname, const std::string& newNickname) {
  //  return formatMessage(":" + currentNickname + " NICK :" + newNickname);
//}

std::string IrcMessageFormatter::nickChange(const std::string& currentNickname, const std::string& username, const std::string& serverIp, const std::string& newNickname) {
    return formatMessage(":" + currentNickname + "!" + username + "@" + serverIp +  " NICK :" + newNickname);
}

// Messages de noms
std::string IrcMessageFormatter::namesReply(const std::string& serverName, const std::string& nick, const std::string& channelName, const std::string& nickList) {
    return formatMessage(":" + serverName + RPL_NAMREPLY + nick + " = " + channelName + " :" + nickList);
}

std::string IrcMessageFormatter::endOfNames(const std::string& serverName, const std::string& nick, const std::string& channelName) {
    return formatMessage(":" + serverName + RPL_ENDOFNAMES + nick + " " + channelName + " :End of /NAMES list.");
}

// Messages de capacités
std::string IrcMessageFormatter::capabilityList(const std::string& serverName, const std::string& nick, const std::string& capabilities) {
    return formatMessage(":" + serverName + " CAP " + nick + " LS :" + capabilities);
}

std::string IrcMessageFormatter::sendMsg(const std::string& sender, const std::string& target, const std::string& message){
    return formatMessage(":" + sender + " PRIVMSG " + target + " :" + message);

}

std::string IrcMessageFormatter::pong(const std::string& message){
    return formatMessage("PONG :" + message);
}
