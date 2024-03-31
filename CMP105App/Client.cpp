#include "Client.h"
#include <iostream>

Client::Client(sf::IpAddress _serverAddress, unsigned short _serverPort) {
	serverAddress = _serverAddress;
	serverPort = _serverPort;

	socket.setBlocking(true);

	//Send test packet to server to be added to server's list of connected clients
	sf::Packet packet;
	packet << PacketCode::AddClient;
	if (socket.send(packet, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to send connection request to server." << std::endl;
	}
	
	//Get client index from server
	sf::IpAddress tempIp;
	unsigned short tempPort;
	sf::Packet data;
	if (socket.receive(data, tempIp, tempPort) != sf::Socket::Done) {
		connectedToServer = false;
		std::cerr << "Failed to connect to server." << std::endl;
	}
	else {
		std::cout << "Successfully connected to server.\n";
		data >> clientIndex;
		connectedToServer = true;
	}

	socket.setBlocking(false);
}


Client::~Client() {
	//Send message to server to tell it to remove client from vector
	socket.setBlocking(true);
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(PacketCode::RemoveClient);
	if (socket.send(outgoingPacket, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to disconnect from server." << std::endl;
	}
	else {
		std::cout << "Successfully disconnected from server.\n";
	}
}


sf::Socket::Status Client::SendDataToClient(int outgoingClientIndex, PacketCode code, sf::Packet incomingPacket) {
	//Combine client index and packet code into the data packet so it can be sent to the server
	sf::Packet outgoingPacket;
	outgoingPacket << static_cast<std::underlying_type<PacketCode>::type>(code) << outgoingClientIndex;
	outgoingPacket.append(incomingPacket.getData(), incomingPacket.getDataSize());
	return socket.send(outgoingPacket, serverAddress, serverPort);
}


sf::Socket::Status Client::SendDataToClient(PacketCode code, sf::Packet incomingPacket) {
	//This function can be called if there are only two clients connected to the server (in which case it will just send data to the other client).
	int outgoingClientIndex = 1 - clientIndex; //0->1, 1->0
	return SendDataToClient(outgoingClientIndex, code, incomingPacket);
}


//To be called every frame
sf::Packet Client::CheckForIncomingDataFromServer() {
	sf::Packet incomingData;

	//Used to check that client is in fact receiving data from the server and not, another client, for example
	sf::IpAddress incomingIp;
	unsigned short incomingPort;

	//Extract data and check if it's empty
	if (socket.receive(incomingData, incomingIp, incomingPort) != sf::Socket::Done) { return sf::Packet(); }

	//Ensuring data is coming from server and not another client
	if ((serverAddress != incomingIp) || (incomingPort != serverPort)) {
		std::cerr << "Client (ip: " << incomingIp << ", port: " << incomingPort << ") is attempting to connect directly to another client." << std::endl;
		std::cerr << "Client should send data through the server instead." << std::endl;
		return sf::Packet();
	}


	return incomingData;

}

int Client::GetClientIndex() {
	return clientIndex;
}
