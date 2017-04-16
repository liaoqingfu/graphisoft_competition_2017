#ifndef SEMIFINAL_CLIENT_CLIENT_HPP
#define SEMIFINAL_CLIENT_CLIENT_HPP

#include "platform_dep.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

template<typename solver>
class client {
	platform_dep::tcp_socket socket_handler;
	std::string received_buffer;
	solver your_solver;
public:
	client(const char host_name[], unsigned short port,
		const char team_name[], const char password[], int task_id) {

		if(!socket_handler.valid()) {
			throw std::runtime_error("Error: Cannot open a socket!");
		}

		hostent* host = gethostbyname(host_name);

		if(!host) {
			throw std::runtime_error("Error: Cannot find host: " + std::string(host_name));
		}

		sockaddr_in socket_address;
		socket_address.sin_family = AF_INET;
		socket_address.sin_port = htons(port);
		socket_address.sin_addr.s_addr = inet_addr(inet_ntoa(*(in_addr *)host->h_addr_list[0]));

		if(connect(socket_handler.get_handler(), (struct sockaddr*)&socket_address,
			sizeof(socket_address)) != 0) {
			throw std::runtime_error("Error: Cannot connect to the server: " +
				std::string(host_name) + ":" + std::to_string(port) +
				" error code: " + std::to_string(socketerrno));
		}

		std::vector<std::string> login_messages;

		login_messages.push_back(std::string("LOGIN ") + team_name + " " + password);

		if(1 <= task_id && task_id <= 10) {
			login_messages.push_back("LEVEL " + std::to_string(task_id));
		}

		send_messages(login_messages);
	}

	void send_messages(const std::vector<std::string>& messages) {
		std::string message;

		for(std::size_t i = 0; i < messages.size(); ++i) {
			message += messages[i] + '\n';
		}
		message += ".\n";

		int sent_bytes = send(socket_handler.get_handler(), message.c_str(), message.size(), 0);

		if(sent_bytes != (int)message.size()) {
			std::cerr << "Warning: Cannot sent message properly: " << message << std::endl;
			std::cerr << sent_bytes << " byte sent from " <<
				message.size() << ". Closing connection." << std::endl;
			socket_handler.invalidate();
		}
	}

	std::vector<std::string> receive_message() {
		std::string buffer(512, '\0');

		int received_bytes = recv(socket_handler.get_handler(), &buffer[0], 512, 0);

		switch(received_bytes) {
		case -1:
			std::cerr << "Error: recv failed!" << std::endl;
		case 0:
			std::cerr << "Connection closed." << std::endl;
			socket_handler.invalidate();
			return std::vector<std::string>();
		}

		std::vector<std::string> result;

		std::stringstream consumer(received_buffer + buffer.c_str());
		while(std::getline(consumer, buffer)) {
            std::cerr << "received: " << std::string{buffer} << std::endl;
			if(buffer == ".") {
				received_buffer = consumer.str().substr(consumer.tellg());
				return result;
			} else if(!buffer.empty()) {
				result.push_back(buffer);
			}
		}

		received_buffer = consumer.str();
		return receive_message();
	}
public:
	void run() {
		std::vector<std::string> tmp = receive_message();

		if(socket_handler.valid()) {
			your_solver.init(tmp);
		}

		while(socket_handler.valid()) {
			tmp = receive_message();
			if(socket_handler.valid()) {
				if(tmp.size() == 1 && tmp[0].find("END") == 0) {
					your_solver.end(tmp[0]);
					break;
				}
				tmp = your_solver.process(tmp);

				if(!tmp.empty()) {
					send_messages(tmp);
				}
			}
		}
		std::cerr << "Game over" << std::endl;
	}
};


#endif // SEMIFINAL_CLIENT_CLIENT_HPP
