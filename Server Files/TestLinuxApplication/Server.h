#ifndef SERVER_H
#define SERVER_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"
#include <vector>


struct ClientInfo
{
	sf::IpAddress ip;
	unsigned short port;

	ClientInfo(sf::IpAddress serverIp, unsigned short serverPort);

	bool operator!=(const ClientInfo& other) const {
		return (ip != other.ip) || (port != other.port);
	}

	bool operator==(const ClientInfo& other) const {
		return (ip == other.ip) && (port == other.port);
	}
};




//Server class for managing clients
class Server : public NetworkNode
{

public:
	Server(sf::IpAddress _ip, unsigned short _port);
	void CheckForIncomingDataFromClient() override;


private:
	std::vector<ClientInfo> connectedClients;
};


#endif