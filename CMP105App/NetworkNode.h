#ifndef NETWORKNODE_H
#define NETWORKNODE_H

#include <SFML/Network.hpp>

//Base class for inherited Server and Client class
class NetworkNode
{
public:
	//For server class
	virtual void CheckForIncomingDataFromClient();

	//For client class
	virtual sf::Packet CheckForIncomingDataFromServer();
	virtual sf::Socket::Status SendDataToClient(int clientIndex, sf::Packet packet);
	virtual sf::Socket::Status SendDataToClient(sf::Packet packet);

protected:
	sf::UdpSocket socket;

	sf::IpAddress serverAddress;
	unsigned short serverPort;
};

#endif