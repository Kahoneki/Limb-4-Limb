#ifndef SERVER_H
#define SERVER_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"
#include <map>

//Server class for managing NetworkManagers
class Server : public NetworkNode
{

public:
	Server(sf::IpAddress _ip, unsigned short _port);

	//----To be called every network tick//
	void CheckForIncomingConnectionRequests();
	void CheckForIncomingDataFromNetworkManager() override;
	//----//

private:
	sf::TcpListener listener;
	std::map<int, sf::TcpSocket> connectedNetworkManagers;
};


#endif