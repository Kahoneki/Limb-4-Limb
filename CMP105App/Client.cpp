#include "Client.h"
#include <iostream>

Client::Client(sf::IpAddress _serverAddress, unsigned short _serverPort) {
	serverAddress = _serverAddress;
	serverPort = _serverPort;

	//Send test packet to server to be added to server's list of connected clients
	sf::Packet packet;
	packet << "Test";
	if (socket.send(packet, serverAddress, serverPort) != sf::Socket::Done) {
		std::cerr << "Failed to connect to server." << std::endl;
		connectedToServer = false;
	}
	else {
		std::cout << "Successfully connected to server.\n";
		connectedToServer = true;
	}
}


sf::Socket::Status Client::SendDataToClient(int clientIndex, sf::Packet packet) {
	//Combine client index into the data packet so it can be sent to the server
	packet << clientIndex;
	return socket.send(packet, serverAddress, serverPort);
}


//To be called every frame
sf::Packet Client::CheckForIncomingData() {

	sf::Packet incomingData;

	//Used to check that client is in fact receiving data from the server and not, another client, for example
	sf::IpAddress incomingIp;
	unsigned short incomingPort;

	//Extract data and check if 
	if (socket.receive(incomingData, incomingIp, incomingPort) != sf::Socket::Done) { return; }

	//Ensuring data is coming from server and not another client
	if ((serverAddress != incomingIp) || (incomingPort != serverPort)) {
		std::cerr << "Client (ip: " << incomingIp << ", port: " << incomingPort << ") is attempting to connect directly to another client." << std::endl;
		std::cerr << "Client should send data through the server instead." << std::endl;
		return;
	}

	return incomingData;

}