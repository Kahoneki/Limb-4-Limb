#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <SFML/Network.hpp>

//Base class for inherited Server and Client class
class NetworkNode
{
protected:
	sf::UdpSocket socket;

	sf::IpAddress serverAddress;
	unsigned short serverPort;
};

#endif