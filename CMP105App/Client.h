#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"

class Client : NetworkNode
{
public:
	Client(sf::IpAddress _serverAddress, unsigned short _serverPort);
	sf::Socket::Status SendDataToClient(int clientIndex, sf::Packet packet);
	sf::Packet CheckForIncomingData();

private:
	bool connectedToServer;
};

#endif