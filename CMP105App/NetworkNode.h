#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <SFML/Network.hpp>
#include "PacketCode.h"

//Base class for inherited Server and NetworkManager class
class NetworkNode
{
public:
	virtual ~NetworkNode();
	//For server class
	virtual void CheckForIncomingDataFromNetworkManager();

	//For NetworkManager class
	virtual void CheckForIncomingDataFromServer();
	virtual sf::Socket::Status SendDataToNetworkManager(int NetworkManagerIndex, sf::Packet packet);
	virtual sf::Socket::Status SendDataToNetworkManager(sf::Packet packet);

protected:
	sf::TcpSocket socket;

	sf::IpAddress serverAddress;
	unsigned short serverPort;
};

#endif