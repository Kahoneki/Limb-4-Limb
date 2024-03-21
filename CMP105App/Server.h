#ifndef SERVER_H
#define SERVER_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"
#include <vector>


struct ClientInfo
{
	sf::IpAddress ip;
	unsigned short port;

};

bool operator!=(ClientInfo a, ClientInfo b) {
	return (a.ip != b.ip) || (a.port != b.port);
}


//Server class for managing clients
class Server : NetworkNode
{

public:
	Server(unsigned short _port);
	void CheckForIncomingData();


private:
	std::vector<ClientInfo> connectedClients;
};


#endif