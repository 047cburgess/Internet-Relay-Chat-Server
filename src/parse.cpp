#include "../include/ft_irc.hpp"

long parse_port(const std::string &port_str) {
		if (port_str.empty()) {
			throw std::invalid_argument("Port number cannot be empty");
		}
		if (port_str.find_first_not_of("0123456789") != std::string::npos) {
			throw std::invalid_argument("Port number must be a valid integer");
		}
		if (port_str.length() > 5) {
			throw std::runtime_error("Port number too long");
		}
		long port = std::strtol(port_str.c_str(), NULL, 10);
		if (port < 0 || port > 65445) {
		 	throw std::out_of_range("Port number out of range");
		 }
		return port;
}

std::string parse_password(const std::string &password) {
	if (password.empty()) {
		throw std::runtime_error("Password cannot be empty");
	}
	if (password.length() > 64) {
		throw std::runtime_error("Password too long, must be less than 64 characters");
	}
	return password;
}