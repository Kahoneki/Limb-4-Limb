#ifndef SERVER_H
#define SERVER_H

#include "SFML/Network.hpp"
#include <vector>


struct ClientInfo
{
	sf::IpAddress ip;
	unsigned short port;
};

//Server class for managing clients
class Server
{
public:
	Server();
	void CheckForIncomingData();

private:
	sf::UdpSocket socket;
	std::vector<ClientInfo> connectedClients;
};

#endif