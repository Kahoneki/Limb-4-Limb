#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"

class Client : public NetworkNode
{
public:
	Client(sf::IpAddress _serverAddress, unsigned short _serverPort);
	~Client();
	sf::Socket::Status SendDataToClient(int clientIndex, sf::Packet incomingPacket);
	sf::Socket::Status SendDataToClient(sf::Packet incomingPacket);
	sf::Packet CheckForIncomingDataFromServer() override;
	
	int GetClientIndex();

private:
	bool connectedToServer;
	int clientIndex;
};

#endif