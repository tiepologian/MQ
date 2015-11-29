# MQ
C++ POSIX Message Queues wrapper

#### Features
* C++11 (lambdas)
* header-only
* multi-threaded
* asynchronous

### Installation
MQ is a header-only library. You don't need to build anything to install it. Simply add the header file to your include path and include "MQ.h":
```c++
#include "MQ.h"
```
The MQ library needs to be linked to the ```rt``` and ```pthread``` libraries, so if you save your file as ```main.cpp```, you can compile it by running:
```bash
g++ --std=c++11 -O3 main.cpp -lrt -lpthread
```
### Usage
##### Server
Create a server endpoint by calling the MQ constructor.
The constructor excepts an ID (```std::string```) and the endpoint type:
```c++
MQ *messageQ = new MQ("example", MQ::EndpointType::Server);
```
Listen for incoming messages by calling the ```listen()``` method, which will call the provided lambda when a message is received:
```c++
messageQ->listen([](std::string message){
    std::cout << "Received: " << message << std::endl;
});
```
Send a message to the client by calling the ```sendMessage()``` method:
```c++
messageQ->sendMessage("Hello from the server!");
```
##### Client
Create a client endpoint by passing the appropriate type to the constructor:
```c++
MQ *client = new MQ("example", MQ::EndpointType::Client);
```
Send a message to the server:
```c++
client->sendMessage("Hello from the client!");
```
Read the response from the server:
```c++
std::cout << "Received: " << client->readMessage() << std::endl;
```
