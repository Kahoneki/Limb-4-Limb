#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"

class Client : public NetworkNode
{
public:
	Client(sf::IpAddress _serverAddress, unsigned short _serverPort);
	sf::Socket::Status SendDataToClient(int clientIndex, sf::Packet packet);
	sf::Socket::Status SendDataToClient(sf::Packet packet);
	sf::Packet CheckForIncomingDataFromServer() override;
	
	int GetClientIndex();

private:
	bool connectedToServer;
	sf::Int16 clientIndex;
};

#endif