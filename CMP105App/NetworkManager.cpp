#include "NetworkManager.h"
#include "BaseNetworkListener.h"
#include <iostream>
#include <type_traits> //For std::underlying_type<T>
#include <chrono> //For epoch timestamps

class OnlinePlayer;


NetworkManager& NetworkManager::getInstance(bool attemptConnection) {
	sf::IpAddress address{ "84.8.144.190" };
	unsigned short port{ 6900 };

	static NetworkManager instance(address, port);

	if (attemptConnection && !instance.connectedToServer) {
		instance.AttemptToConnectToServer();
	}

	return instance;
	
}


NetworkManager& NetworkManager::getInstance(bool attemptConnection, sf::IpAddress _serverAddress, unsigned short _serverPort) {
	static NetworkManager instance(_serverAddress, _serverPort);	
	return instance;
}


NetworkManager::NetworkManager(sf::IpAddress _serverAddress, unsigned short _serverPort) {
	serverAddress = _serverAddress;
	serverPort = _serverPort;

	connectedToServer = false;
	networkManagerIndex = -1;

	//Initialise networkListeners to x null pointers where x is the number of entities that hold a reserved spot within the vector
	for (int i{ 0 }; i < ReservedEntityIndexTable::NUM_RESERVED_ENTITIES; ++i) {
		networkListeners.push_back(nullptr);
	}

}


NetworkManager::~NetworkManager() {
	if (connectedToServer) {
		AttemptToDisconnectFromServer();
	}
}


bool NetworkManager::AttemptToConnectToServer()
{
	//Connect to server
	tcpSocket.setBlocking(true);
	if (tcpSocket.connect(serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to send connection request to server." << std::endl;
	}

	//Get NetworkManager index from server
	sf::Packet incomingData;
	if (tcpSocket.receive(incomingData) != sf::Socket::Done) {
		connectedToServer = false;
		std::cerr << "Failed to connect to server." << std::endl;
	}
	else {
		std::cout << "Successfully connected to server.\n";
		incomingData >> networkManagerIndex;
		connectedToServer = true;
	}

	tcpSocket.setBlocking(false);
	udpSocket.setBlocking(false);
	return connectedToServer;
}

bool NetworkManager::AttemptToDisconnectFromServer()
{
	//Send message to server to tell it to remove NetworkManager from vector
	tcpSocket.setBlocking(true);
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(PacketCode::RemoveNetworkManager);
	if (tcpSocket.send(outgoingPacket) != sf::Socket::Done) {
		std::cerr << "Failed to disconnect from server." << std::endl;
		return false;
	}
	else {
		std::cout << "Successfully disconnected from server.\n";
		connectedToServer = false;
		return true;
	}
}



void NetworkManager::SendDataToNetworkManager(int outgoingNetworkManagerIndex, int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket) {
	//Combine Packet code, NetworkManager index, and NetworkListener index into the data packet so it can be sent to the server

	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(packetCode) << outgoingNetworkManagerIndex << networkListenerIndex;
	switch (packetCode)
	{

	case PacketCode::KeyChange:
	{
		bool pressed;
		int key;
		incomingPacket >> pressed >> key;
		outgoingPacket << pressed << key;
		tcpSocket.send(outgoingPacket);
		break;
	}

	case PacketCode::PositionChange:
	{
		sf::Vector2f pos;
		incomingPacket >> pos.x >> pos.y;
		outgoingPacket << pos.x << pos.y;
		udpSocket.send(outgoingPacket, serverAddress, serverPort);

		break;
	}
	}
	namespace c = std::chrono;
	uint64_t ms = c::duration_cast<c::milliseconds>(c::system_clock::now().time_since_epoch()).count();
	std::cout << "Send: " << ms << " milliseconds since the epoch\n";
}


void NetworkManager::SendDataToNetworkManager(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket) {
	//This function can be used if there are only two NetworkManagers connected to the server (in which case it will just send data to the other NetworkManager).
	int outgoingNetworkManagerIndex = 1 - networkManagerIndex; //0->1, 1->0
	SendDataToNetworkManager(outgoingNetworkManagerIndex, networkListenerIndex, packetCode, incomingPacket);
}


void NetworkManager::SendDataToServer(int networkListenerIndex, PacketCode packetCode, sf::Packet incomingPacket) {
	//This function is to be used if the network manager is sending data to the server, but not necessarily to be passed on to another network manager
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(packetCode);
	if (networkListenerIndex != -1) {
		outgoingPacket << networkListenerIndex;
	}
	switch (packetCode)
	{
	case PacketCode::UsernameRegister:
	{
		std::string username;
		incomingPacket >> username;
		outgoingPacket << username;
		tcpSocket.send(outgoingPacket);
		break;
	}
	case PacketCode::Login:
	{
		std::string username;
		sf::Uint64 uuid;
		incomingPacket >> username >> uuid;
		outgoingPacket << username << uuid;
		tcpSocket.send(outgoingPacket);
		break;
	}
	case PacketCode::Logout:
	{
		tcpSocket.send(outgoingPacket);
		break;
	}
	case PacketCode::UsernameInvite:
	{
		std::string username;
		incomingPacket >> username;
		outgoingPacket << username;
		tcpSocket.send(outgoingPacket);
		break;
	}
	case PacketCode::MatchAcceptanceClientToServer:
	{
		bool acceptance;
		int networkManagerIndex;
		incomingPacket >> acceptance >> networkManagerIndex;
		outgoingPacket << acceptance << networkManagerIndex;
		tcpSocket.send(outgoingPacket);
		break;
	}
	}
	namespace c = std::chrono;
	uint64_t ms = c::duration_cast<c::milliseconds>(c::system_clock::now().time_since_epoch()).count();
	std::cout << "Send: " << ms << " milliseconds since the epoch\n";
}


//To be called every network tick
void NetworkManager::CheckForIncomingDataFromServer() {
	//----TCP DATA----//
	{
		sf::Packet incomingData;

		//Extract data and check if it's empty
		if (tcpSocket.receive(incomingData) == sf::Socket::Done) {
			namespace c = std::chrono;
			uint64_t ms = c::duration_cast<c::milliseconds>(c::system_clock::now().time_since_epoch()).count();
			std::cout << "TCP Receive: " << ms << " milliseconds since the epoch\n";

			//Extract networkListenerIndex and send rest of packet to appropriate network listener
			int networkListenerIndex;
			incomingData >> networkListenerIndex;
			if (networkListeners[networkListenerIndex] != nullptr) {
				networkListeners[networkListenerIndex]->InterpretPacket(incomingData);
			}
		}
	}
	//----------------//


	//----UDP DATA----//
	{
		sf::Packet incomingData;

		//Extract data and check if it's coming from the server (not another network manager)
		sf::IpAddress incomingAddress;
		unsigned short incomingPort;
		if (udpSocket.receive(incomingData, incomingAddress, incomingPort) == sf::Socket::Done) {
			if (incomingAddress == serverAddress && incomingPort == serverPort) {
				namespace c = std::chrono;
				uint64_t ms = c::duration_cast<c::milliseconds>(c::system_clock::now().time_since_epoch()).count();
				std::cout << "UDP Receive: " << ms << " milliseconds since the epoch\n";

				//Extract networkListenerIndex and send rest of packet to appropriate network listener
				int networkListenerIndex;
				incomingData >> networkListenerIndex;
				if (networkListeners[networkListenerIndex] != nullptr) {
					networkListeners[networkListenerIndex]->InterpretPacket(incomingData);
				}
			}
			else {
				std::cerr << "NetworkManager (" << incomingAddress << ", " << incomingPort << ") is attempting to send data directly to this NetworkManager" << std::endl;
			}
		}
	}
	//----------------//
}



int NetworkManager::getNetworkManagerIndex() { return networkManagerIndex; }
bool NetworkManager::getConnectedToServer() { return connectedToServer; }

void NetworkManager::sendNums() {
	for (int i{ 1 }; i <= 5; ++i) {
		sf::Packet p;
		sf::Packet outgoingPacket;
		outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(PacketCode::Nums) << 1 << i;
		std::cout << udpSocket.send(outgoingPacket, serverAddress, serverPort) << '\n';
		std::cout << i << '\n';
	}
	std::cout << '\n';
}