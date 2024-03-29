#include "Server.h"
#include <algorithm> //For std::find
#include <iostream>


ClientInfo::ClientInfo(sf::IpAddress serverIp, unsigned short serverPort) {
	ip = serverIp;
	port = serverPort;
}


Server::Server(sf::IpAddress _ip, unsigned short _port) {
	serverAddress = _ip;
	serverPort = _port;

	socket.setBlocking(false);

	if (socket.bind(serverPort) != sf::Socket::Done) { std::cerr << "Server failed to bind to port " << serverPort << std::endl; }
	else { std::cout << "Server successfully bound to port " << serverPort << '\n'; }

}


void Server::CheckForIncomingDataFromClient() {
	sf::Packet incomingData;
	sf::IpAddress incomingClientAddress;
	unsigned short incomingClientPort;

	if (socket.receive(incomingData, incomingClientAddress, incomingClientPort) != sf::Socket::Done) { return; }

	std::cout << "Client data incoming (top of func.)\n";

	ClientInfo incomingClient {incomingClientAddress, incomingClientPort};

	std::underlying_type<PacketCode>::type packetCode;
	incomingData >> packetCode;
	switch (packetCode)
	{
	case PacketCode::AddClient:
	{
		//Double check that client isn't already in vector
		if (std::find(connectedClients.begin(), connectedClients.end(), incomingClient) != connectedClients.end()) { return; }
		connectedClients.push_back(incomingClient);
		std::cout << "Client (ip: " << incomingClient.ip << ", " << incomingClient.port << ") connected to server.\n";

		//Send client index back to client
		sf::Packet data;
		data << (int)connectedClients.size() - 1;
		if (socket.send(data, incomingClient.ip, incomingClient.port) != sf::Socket::Done) { std::cerr << "Failed to send client index to client." << std::endl; }
		else { std::cerr << "Successfully sent client index to client.\n"; }

		return;
	}

	case PacketCode::RemoveClient:
	{
		std::cout << "Client (ip: " << incomingClient.ip << ", " << incomingClient.port << ") disconnected from server.\n";
		connectedClients.erase(std::find(connectedClients.begin(), connectedClients.end(), incomingClient));
		return;
	}
	default:
	{
		std::cout << "I am  receiving client data\n";
		//This case handles data that does not need checks performed on it, in this case it should just be sent straight to the client

		//Add packet code to outgoing data
		sf::Packet outgoingData;
		outgoingData << packetCode;

		//Separate packet and client index from incoming data
		int clientIndex;
		incomingData >> clientIndex;

		//Append the incoming data to outgoing data (now contains a packet code and corresponding data)
		outgoingData.append(static_cast<const char*>(incomingData.getData()) + sizeof(clientIndex), incomingData.getDataSize() - sizeof(clientIndex));

		//Validate data (make sure client is trying to send data to an ip+port that is in the array and make sure client isn't trying to send themselves data. possibly other checks also.)
		if ((clientIndex >= connectedClients.size()) || (connectedClients[clientIndex] == incomingClient)) {
			std::cerr << "Client (ip: " << incomingClient.ip << ", " << incomingClient.port << ") tried to send a message to an invalid client (ip: "
				<< connectedClients[clientIndex].ip << ", " << connectedClients[clientIndex].port << ")!" << std::endl;
			return;
		}

		//Send data to client
		socket.send(outgoingData, connectedClients[clientIndex].ip, connectedClients[clientIndex].port);

		return;
	}
	}
}
