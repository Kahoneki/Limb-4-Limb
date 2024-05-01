#ifndef SERVER_H
#define SERVER_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"
#include <map>

//Server class for managing NetworkManagers
class Server
{

public:
	Server(sf::IpAddress _ip, unsigned short _port);

	//----To be called every network tick//
	void CheckForIncomingConnectionRequests();
	void CheckForIncomingTCPData();
	void CheckForIncomingUDPData();
	//----//

private:
	sf::IpAddress serverAddress;
	unsigned short serverPort;

	sf::TcpListener tcpListener;
	sf::TcpSocket tcpSocket;
	sf::UdpSocket udpSocket;
	std::map<int, sf::TcpSocket> connectedNetworkManagers; //Using a map instead of a vector for the unique property that map[map.size()] will create a new item, this is to work around sockets not being copyable
};


#endif