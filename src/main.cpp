#include "../include/ft_irc.hpp"

int main(int ac, char **av) {

	if (ac != 3) {
		std::cerr << "Usage: ./ft_irc <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}

	try {

		std::string port_str = av[1];
		long port = parse_port(port_str);
		std::string password = parse_password(av[2]);

		Server server(port, password);

		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		server.init();
		server.run();
	
	} catch (const std::exception& e) {
				std::cerr << e.what() << std::endl;
	}
}
