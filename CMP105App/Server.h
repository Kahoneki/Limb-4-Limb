#ifndef SERVER_H
#define SERVER_H

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
class Server
{

public:
	Server(unsigned short _port);
	void CheckForIncomingData();


private:
	sf::UdpSocket socket;
	
	sf::IpAddress ip;
	unsigned short port;

	std::vector<ClientInfo> connectedClients;

};


#endif