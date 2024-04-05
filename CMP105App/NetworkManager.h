#ifndef NETWORKMANAGER
#define NETWORKMANAGER_H

#include "NetworkNode.h"
#include "SFML/Network.hpp"
#include <vector>

template<typename ParentType>
class NetworkListener;

class NetworkManager : public NetworkNode
{
public:
	static NetworkManager& getInstance();
	static NetworkManager& getInstance(sf::IpAddress _serverAddress, unsigned short _serverPort);
	~NetworkManager();
	template <typename ParentType>
	void AddNetworkListener(NetworkListener<void>* nl);
	sf::Socket::Status SendDataToNetworkManager(int outgoingNetworkManagerIndex, int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	sf::Socket::Status SendDataToNetworkManager(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket);
	sf::Packet CheckForIncomingDataFromServer() override;
	
	int GetNetworkManagerIndex();

private:
	NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort);

	bool connectedToServer;
	int networkManagerIndex;
	
	int reservedEntities; //Number of entities that hold a reserved spot in the networkListeners vector, this is to ensure that, e.g.:, players 1-4 are always in indices 0-3 and in the correct order
	std::vector<NetworkListener<void>*> networkListeners;
};

#endif