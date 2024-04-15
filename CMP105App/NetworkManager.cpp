#include "NetworkManager.h"
#include "BaseNetworkListener.h"
#include <iostream>
#include <type_traits> //For std::is_same()

class OnlinePlayer;


NetworkManager& NetworkManager::getInstance() {
	sf::IpAddress address{ "limbforlimb.duckdns.org" };
	unsigned short port{ 6900 };
	static NetworkManager instance(address, port);
	return instance;
}


NetworkManager& NetworkManager::getInstance(sf::IpAddress _serverAddress, unsigned short _serverPort) {
	static NetworkManager instance(_serverAddress, _serverPort);
	return instance;
}


NetworkManager::NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort) {
	serverAddress = _serverAddress;
	serverPort = _serverPort;

	socket.setBlocking(true);

	//Send test packet to server to be added to server's list of connected NetworkManagers
	sf::Packet packet;
	packet << PacketCode::AddNetworkManager;
	if (socket.send(packet, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to send connection request to server." << std::endl;
	}
	
	//Get NetworkManager index from server
	sf::IpAddress tempIp;
	unsigned short tempPort;
	sf::Packet data;
	if (socket.receive(data, tempIp, tempPort) != sf::Socket::Done) {
		connectedToServer = false;
		std::cerr << "Failed to connect to server." << std::endl;
	}
	else {
		std::cout << "Successfully connected to server.\n";
		data >> networkManagerIndex;
		connectedToServer = true;
	}

	socket.setBlocking(false);

	//Initialise networkListeners to x null pointers where x is the number of entities that hold a reserved spot within the vector
	reservedEntities = 6;
	for (int i{ 0 }; i < reservedEntities; ++i) {
		networkListeners.push_back(nullptr);
	}
}


NetworkManager::~NetworkManager() {
	//Send message to server to tell it to remove NetworkManager from vector
	socket.setBlocking(true);
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(PacketCode::RemoveNetworkManager);
	if (socket.send(outgoingPacket, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to disconnect from server." << std::endl;
	}
	else {
		std::cout << "Successfully disconnected from server.\n";
	}
}




sf::Socket::Status NetworkManager::SendDataToNetworkManager(int outgoingNetworkManagerIndex, int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket) {
	//Combine Packet code, NetworkManager index, and NetworkListener index into the data packet so it can be sent to the server
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(packetCode) << outgoingNetworkManagerIndex << networkListenerIndex;
	switch (packetCode)
	{

	case PacketCode::Verification:
	{
		sf::Vector2f pos;
		sf::Int16 health;
		bool activeLimbs[4];
		incomingPacket >> pos.x >> pos.y >> health >> activeLimbs[0] >> activeLimbs[1] >> activeLimbs[2] >> activeLimbs[3];
		outgoingPacket << pos.x << pos.y << health << activeLimbs[0] << activeLimbs[1] << activeLimbs[2] << activeLimbs[3];
		break;
	}

	case PacketCode::KeyChange:
	{
		bool pressed;
		int key;
		incomingPacket >> pressed >> key;
		outgoingPacket << pressed << key;
		break;
	}

	}
	
	return socket.send(outgoingPacket, serverAddress, serverPort);
}


sf::Socket::Status NetworkManager::SendDataToNetworkManager(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket) {
	//This function can be called if there are only two NetworkManagers connected to the server (in which case it will just send data to the other NetworkManager).
	int outgoingNetworkManagerIndex = 1 - networkManagerIndex; //0->1, 1->0
	return SendDataToNetworkManager(outgoingNetworkManagerIndex, networkListenerIndex, packetCode, incomingPacket);
}


//To be called every network tick
void NetworkManager::CheckForIncomingDataFromServer() {
	sf::Packet incomingData;

	//Used to check that NetworkManager is in fact receiving data from the server and not, another NetworkManager, for example
	sf::IpAddress incomingIp;
	unsigned short incomingPort;

	//Extract data and check if it's empty
	std::cout << "I AM RECEIVING DATA\n";
	if (socket.receive(incomingData, incomingIp, incomingPort) != sf::Socket::Done) { return; }


	//Ensuring data is coming from server and not another NetworkManager
	if ((serverAddress != incomingIp) || (incomingPort != serverPort)) {
		std::cerr << "NetworkManager (ip: " << incomingIp << ", port: " << incomingPort << ") is attempting to connect directly to another NetworkManager." << std::endl;
		std::cerr << "NetworkManager should send data through the server instead." << std::endl;
		return;
	}

	//Extract networkListenerIndex and send rest of packet to appropriate network listener
	int networkListenerIndex;
	incomingData >> networkListenerIndex;
	if (networkListeners[networkListenerIndex] != nullptr) {
		networkListeners[networkListenerIndex]->InterpretPacket(incomingData);
	}
}

int NetworkManager::GetNetworkManagerIndex() {
	return networkManagerIndex;
}
