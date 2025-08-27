#include "Server.class.hpp"

# include <sstream>
# include <set>
# include <ctime>
# include <time.h>
# include <iomanip>


void	Server::handleBot(Client *client, std::vector<std::string> args)
{
    if (!client->isRegistered())
    {
        std::string response = IrcMessageFormatter::notRegistered(_serverName);
        send(client->getSocket(), response.c_str(), response.size(), 0);
        return ;
    }

    if (args.size() < 1)
    {
        std::string response = IrcMessageFormatter::needMoreParams(_serverName, "BOT");
        send(client->getSocket(), response.c_str(), response.size(), 0);
        return;
    }    

    if (args[0] == "TIME")
    {
        const time_t now = time(0);
        struct tm *ltm = localtime(&now);
        std::ostringstream oss;
        oss << "Current time is: "
            << std::setfill('0') << std::setw(2) << ltm->tm_hour << ":"
            << std::setfill('0') << std::setw(2) << ltm->tm_min << ":"
            << std::setfill('0') << std::setw(2) << ltm->tm_sec;
        std::string response = IrcMessageFormatter::genericError(_serverName, "BOT", oss.str());
        send(client->getSocket(), response.c_str(), response.size(), 0);
    }
    else
    {
        std::string response = IrcMessageFormatter::unknownCommand(_serverName, args[1]);
        send(client->getSocket(), response.c_str(), response.size(), 0);
    }
}

void Server::handleRockPaperScissors(Client *client, std::vector<std::string> args)
{
    if (!client->isRegistered())
    {
        std::string response = IrcMessageFormatter::notRegistered(_serverName);
        send(client->getSocket(), response.c_str(), response.size(), 0);
        return ;
    }

    if (args.size() < 2 || args[0] != "RPS")
    {
        std::string response = IrcMessageFormatter::genericError(_serverName, "BOT", 
            "Usage: BOT RPS <rock|paper|scissors>");
        send(client->getSocket(), response.c_str(), response.size(), 0);
        return;
    }

    std::string playerMove = args[1];
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(playerMove.begin(), playerMove.end(), playerMove.begin(), ::tolower);
    
    // Validate player move
    if (playerMove != "rock" && playerMove != "paper" && playerMove != "scissors")
    {
        std::string response = IrcMessageFormatter::genericError(_serverName, "BOT", 
            "Invalid move! Use: rock, paper, or scissors");
        send(client->getSocket(), response.c_str(), response.size(), 0);
        return;
    }

    // Generate bot move
    srand(time(NULL));
    int botChoice = rand() % 3;
    std::string botMove;
    
    switch (botChoice)
    {
        case 0:
            botMove = "rock";
            break;
        case 1:
            botMove = "paper";
            break;
        case 2:
            botMove = "scissors";
            break;
    }

    // Determine winner
    std::string result;
    std::string gameResult;
    
    if (playerMove == botMove)
    {
        result = "It's a tie!";
        gameResult = "TIE";
    }
    else if ((playerMove == "rock" && botMove == "scissors") ||
             (playerMove == "paper" && botMove == "rock") ||
             (playerMove == "scissors" && botMove == "paper"))
    {
        result = "You win!";
        gameResult = "WIN";
    }
    else
    {
        result = "Bot wins!";
        gameResult = "LOSE";
    }

    // Format response message
    std::ostringstream oss;
    oss << "🎮 ROCK PAPER SCISSORS 🎮\n";
    oss << "Your move: " << playerMove << "\n";
    oss << "Bot's move: " << botMove << "\n";
    oss << "Result: " << result;
    
    // Add some fun emojis based on result
    if (gameResult == "WIN")
        oss << " 🎉";
    else if (gameResult == "LOSE")
        oss << " 😔";
    else
        oss << " 🤝";

    std::string response = IrcMessageFormatter::genericError(_serverName, "BOT", oss.str());
    send(client->getSocket(), response.c_str(), response.size(), 0);
}
