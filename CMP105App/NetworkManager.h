#ifndef NETWORKMANAGER
#define NETWORKMANAGER_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"

class NetworkManager : public NetworkNode
{
public:
	NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort);
	~NetworkManager();
	sf::Socket::Status SendDataToNetworkManager(int NetworkManagerIndex, PacketCode code, sf::Packet incomingPacket);
	sf::Socket::Status SendDataToNetworkManager(PacketCode code, sf::Packet incomingPacket);
	sf::Packet CheckForIncomingDataFromServer() override;
	
	int GetNetworkManagerIndex();

private:
	bool connectedToServer;
	int NetworkManagerIndex;
};

#endif