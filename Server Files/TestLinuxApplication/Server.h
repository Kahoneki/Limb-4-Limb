#ifndef SERVER_H
#define SERVER_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"
#include <vector>


struct NetworkManagerInfo
{
	sf::IpAddress ip;
	unsigned short port;

	NetworkManagerInfo(sf::IpAddress serverIp, unsigned short serverPort);

	bool operator!=(const NetworkManagerInfo& other) const {
		return (ip != other.ip) || (port != other.port);
	}

	bool operator==(const NetworkManagerInfo& other) const {
		return (ip == other.ip) && (port == other.port);
	}
};




//Server class for managing NetworkManagers
class Server : public NetworkNode
{

public:
	Server(sf::IpAddress _ip, unsigned short _port);
	void CheckForIncomingDataFromNetworkManager() override;


private:
	std::vector<NetworkManagerInfo> connectedNetworkManagers;
};


#endif