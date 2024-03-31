#ifndef NetworkNode_H
#define NetworkNode_H

#include <SFML/Network.hpp>

//Used for encoding the type of data a packet contains
enum PacketCode
{
	AddNetworkManager,
	RemoveNetworkManager,
	PositionChange
};


//Base class for inherited Server and NetworkManager class
class NetworkNode
{
public:
	virtual ~NetworkNode();
	//For server class
	virtual void CheckForIncomingDataFromNetworkManager();

	//For NetworkManager class
	virtual sf::Packet CheckForIncomingDataFromServer();
	virtual sf::Socket::Status SendDataToNetworkManager(int NetworkManagerIndex, sf::Packet packet);
	virtual sf::Socket::Status SendDataToNetworkManager(sf::Packet packet);

protected:
	sf::UdpSocket socket;

	sf::IpAddress serverAddress;
	unsigned short serverPort;
};

#endif