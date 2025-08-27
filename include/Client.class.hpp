#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include <string>
#include <unistd.h>
#include <set>
#include <ctime>
#include <sys/socket.h>

#include "../include/Channel.class.hpp"

class Channel;

class Client
{

    private:
        int _socket;
        std::string _ipAddr;

        // ID DU CLIENT
        std::string _nickname;
        std::string _username;
        std::string _hostname;
        std::string _servername;
        std::string _realname;

        /* Tampon pour stocker les messages partiels */
        std::string _messageBuffer;

        // STATUS DU CLIENT
        bool _registered;
        bool _sentPassword;
        bool _sentNickname;
        bool _sentUsername;
        bool _isAway;
        bool _isOperator;

        // CANAUX DU CLIENT
        std::vector<Channel*> _channels;


        /* Temps du dernier pong */
        time_t _lastPongTime;

        /* Temps de la dernière activité */
        time_t _lastActivityTime;

        /* Indique si le client a reçu un PING */
        bool pingReceived;

    public:

        Client(int socket, char* ipAddr);
        Client();
        ~Client();

        int getSocket() const;
        bool isRegistered() const;
        void setRegistered(bool status);
        bool hasSentPassword() const;
        void setSentPassword(bool status);
        bool hasSentNickname() const;
        void setSentNickname(bool status);
        bool hasSentUsername() const;
        void setSentUsername(bool status);
	    bool readyToRegister() const;

        // STATUS DU CLIENT
        bool isAway() const;
        void setAway(bool status);
        bool isOperator() const;
        void setOperator(bool status);
        time_t getLastActivityTime() const;
        void updateLastActivity();

        // IDENTITE DU CLIENT
        const std::string &getNickname() const;
        void setNickname(const std::string &nickname);
        const std::string &getUsername() const;
        void setUsername(const std::string &username);
        const std::string &getHostname() const;
        void setHostname(const std::string &hostname);
        const std::string &getServername() const;
        void setServername(const std::string &servername);
        const std::string &getRealname() const;
        void setRealname(const std::string &realname);
        const std::string &getIp() const;

        // A PROPOS DES CANAUX DU CLIENT
        void joinChannel(Channel *channel);
        void leaveChannel(Channel *channel);
        bool isInChannel(const std::string &channelName) const;
        const std::vector<Channel*> &getChannelsList() const;



        /*   -'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-',-'   */
        /*                                  PONGTIME                                 */
        /*   -'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-'-,-',-'   */

        /* Définit le temps du dernier pong */
        void setLastPongTime(time_t time);

        /* Retourne le temps du dernier pong */
        time_t getLastPongTime() const;

        /* Getter pour le tampon de messages partiels */
        std::string & getMessageBuffer();

        /* Indique si le client a reçu un PING */
        bool isPingReceived() const;
        void setPingReceived(bool status);


};

#endif
