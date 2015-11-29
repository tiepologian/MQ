/*
 * main.cpp
 *
 *  Created on: Nov 29, 2015
 *      Author: Gianluca Tiepolo <tiepolo.gian@gmail.com>
 */

#include "MQ.h"
#include <chrono>

int main(int argc, char *argv[]) {
	if(argc < 2) {
		std::cout << "Usage: ./example client|server" << std::endl;
		return 1;
	}

	MQ *messageQ;

	if(std::string(argv[1]) == "server") {
		messageQ = new MQ("example", MQ::EndpointType::Server);
		messageQ->listen([messageQ](std::string message){
			std::cout << "Received: " << message << std::endl;
			messageQ->sendMessage("Hello from the server!");
		});
		while(true) std::this_thread::sleep_for (std::chrono::seconds(1));
	} else if(std::string(argv[1]) == "client") {
		messageQ = new MQ("example", MQ::EndpointType::Client);
		std::string data;
		std::cout << "Write message to send and press ENTER:" << std::endl;
		getline(std::cin, data);
		messageQ->sendMessage(data);
		std::cout << "Received: " << messageQ->readMessage() << std::endl;
	}

	delete messageQ;
	return 0;
}
