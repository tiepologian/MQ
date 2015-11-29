/*
 * MQ.h
 *
 *  Created on: Nov 29, 2015
 *      Author: Gianluca Tiepolo <tiepolo.gian@gmail.com>
 */

#ifndef MQ_H_
#define MQ_H_

#include <iostream>
#include <functional>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <thread>

/*
 * MessageQueue Class
 */
class MQ {
public:
	/*
	 * Endpoint types
	 */
	enum EndpointType {
		Client,
		Server
	};
	/*
	 * Constructor - Constructs new message queue with specified ID
	 */
	MQ(std::string id, EndpointType type) : _type(type) {
		std::string name1 = "/" + id + "_1";
		std::string name2 = "/" + id + "_2";
		if (type == EndpointType::Server) {
			// endpoint acts as the 'server', so clear Message Queue and create new one
			mq_unlink(name1.c_str());
			mq_unlink(name2.c_str());
		}
		_mqd1 = mq_open(name1.c_str(), O_RDWR | O_CREAT, 0664, 0);
		_mqd2 = mq_open(name2.c_str(), O_RDWR | O_CREAT, 0664, 0);
		if ((_mqd1 == -1) || (_mqd2 == -1)) {
			std::cout << "Error opening message queue" << std::endl;
			exit(1);
		}
		mq_getattr(_mqd1, &_attr1);
		mq_getattr(_mqd2, &_attr2);
	}

	/*
	 * Destructor - Closes Message Queue
	 */
	virtual ~MQ() {
		mq_close(_mqd1);
		mq_close(_mqd2);
	}

	/*
	 * Listens for incoming messages and passes them to the provided lambda callback.
	 * This is an ASYNC method and it runs in a separate thread, so this is non-blocking.
	 */
	void listen(std::function<void(std::string)> cb) {
		if (_type == EndpointType::Client) {
			std::cout << "ERROR: Cannot call listen() on client" << std::endl;
			exit(1);
		}
		std::thread t([this, cb]() {
			while(true) {
				// Default max message size is set to 8192 bytes, so we create a 9000 byte buffer
				char *buffer = new char[9000];
				memset(buffer, '\0', 9000);
				if(mq_receive(_mqd2, buffer, 9000, &_prio) == -1) {
					std::cout << "Error reading message" << std::endl;
					exit(1);
				}
				cb(std::string(buffer));
				delete [] buffer;
			}
		});
		t.detach();
	}

	/*
	 * Sends message through Message Queue.
	 */
	void sendMessage(std::string data) {
		int result;
		if (_type == EndpointType::Server)
			result = mq_send(_mqd1, data.c_str(), strlen(data.c_str()), _prio);
		else
			result = mq_send(_mqd2, data.c_str(), strlen(data.c_str()), _prio);
		if (result == -1) {
			std::cout << "ERROR: Can't send message" << std::endl;
			exit(1);
		}
	}

	/*
	 * Reads and returns one message synchronously from the Message Queue.
	 * This is a SYNC method, so it will block if there is no data to read
	 * until a message becomes available.
	 */
	std::string readMessage() {
		// Default max message size is set to 8192 bytes, so we create a 9000 byte buffer
		char *buffer = new char[9000];
		memset(buffer, '\0', 9000);
		if (mq_receive(_mqd1, buffer, 9000, &_prio) == -1) {
			std::cout << "Error reading message" << std::endl;
			exit(1);
		}
		std::string message(buffer);
		delete [] buffer;
		return message;
	}
private:
	EndpointType _type;
	mqd_t _mqd1;
	mqd_t _mqd2;
	unsigned int _prio = 1;
	struct mq_attr _attr1;
	struct mq_attr _attr2;
};

#endif /* MQ_H_ */
