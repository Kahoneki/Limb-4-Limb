#ifndef CLIENT_H
#define CLIENT_H

#include "SFML/Network.hpp"

class Client
{
public:
	Client(sf::IpAddress _serverAddress, unsigned short _serverPort);
	sf::Socket::Status SendDataToClient(int clientIndex, sf::Packet packet);
	sf::Packet CheckForIncomingData();

private:

	sf::UdpSocket socket;

	sf::IpAddress serverAddress;
	unsigned short serverPort;

	bool connectedToServer;
};

#endif