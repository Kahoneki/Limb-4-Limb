#include "Server.h"
#include <algorithm> //For std::find
#include <iostream>

#include "sqlite3.h"


Server::Server(sf::IpAddress _ip, unsigned short _port) {
	serverAddress = _ip;
	serverPort = _port;

	listener.setBlocking(false);

	if (listener.listen(serverPort) != sf::Socket::Done) { std::cerr << "Server failed to bind to port " << serverPort << std::endl; }
	else { std::cout << "Server successfully bound to port " << serverPort << '\n'; }
}


void Server::CheckForIncomingConnectionRequests() {
	//Create new socket and check if there's a connection request
	connectedNetworkManagers[connectedNetworkManagers.size()].setBlocking(false);
	if (listener.accept(connectedNetworkManagers[connectedNetworkManagers.size()-1]) == sf::Socket::Done) {

		//Check if socket is already connected by searching through all ips
		int foundIndex{ -1 };
		for (int i{ 0 }; i < connectedNetworkManagers.size(); ++i) {
			if (connectedNetworkManagers[i].getRemoteAddress() == connectedNetworkManagers[connectedNetworkManagers.size() - 1].getRemoteAddress()) {
				foundIndex = i;
			}
		}

		std::cout << "Accepted socket at ip: " << connectedNetworkManagers[connectedNetworkManagers.size()-1].getRemoteAddress() << ", port: " << connectedNetworkManagers[connectedNetworkManagers.size()-1].getRemotePort() << '\n';

		//Send network manager index back to network manager
		sf::Packet outgoingPacket;
		int networkManagerIndex{ static_cast<int>(connectedNetworkManagers.size() - 1) };
		outgoingPacket << networkManagerIndex;
		connectedNetworkManagers[connectedNetworkManagers.size()-1].send(outgoingPacket);
	}
	else {
		//No connection request, remove from map
		connectedNetworkManagers.erase(connectedNetworkManagers.size() - 1);
	}
}


void Server::CheckForIncomingDataFromNetworkManager() {
	//Loop through all connected sockets to see if data is being received
	for (int i{ 0 }; i < connectedNetworkManagers.size(); ++i) {
		sf::Packet incomingData;
		if (connectedNetworkManagers[i].receive(incomingData) != sf::Socket::Done) { continue; } //No data being received from this socket, continue to the next one
		std::underlying_type_t<PacketCode> packetCode;
		incomingData >> packetCode;
		switch (packetCode)
		{
		case PacketCode::RemoveNetworkManager:
		{
			std::cout << "PacketCode: RemoveNetworkManager\n";
			std::cout << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ") disconnected from server.\n";
			connectedNetworkManagers.erase(i);
			break;
		}
		case PacketCode::AccountRegistrationRequest:
		{
			std::cout << "PacketCode: AccountRegistrationRequest\n";

			break;
		}
		case PacketCode::KeyChange:
		{
			std::cout << "PacketCode: KeyChange\n";

			//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
			int networkManagerIndex;
			int networkListenerIndex;
			int key;
			bool pressed;
			incomingData >> networkManagerIndex >> networkListenerIndex >> pressed >> key;

			//Add networkListenerIndex, packetCode, and data to outgoingData
			sf::Packet outgoingData;
			outgoingData << networkListenerIndex << packetCode << pressed << key;

			//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data)
			if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex].getRemoteAddress() == connectedNetworkManagers[i].getRemoteAddress())) {
				std::cerr << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ") tried to send a message to an invalid NetworkManager (ip: "
					<< connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << ", " << connectedNetworkManagers[networkManagerIndex].getRemotePort() << ")!" << std::endl;
				continue;
			}

			//Send data to NetworkManager
			std::cout << "\n\n( " << connectedNetworkManagers[i].getRemoteAddress() << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << "\n\n";
			connectedNetworkManagers[networkManagerIndex].send(outgoingData);

			break;
		}
		case PacketCode::PositionChange:
		{
			std::cout << "PacketCode: PositionChange\n";


			//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
			int networkManagerIndex;
			int networkListenerIndex;
			sf::Vector2f pos;
			incomingData >> networkManagerIndex >> networkListenerIndex >> pos.x >> pos.y;
			std::cout << pos.x << ' ' << pos.y << '\n';

			//Add networkListenerIndex, packetCode, and data to outgoingData
			sf::Packet outgoingData;
			outgoingData << networkListenerIndex << packetCode << pos.x << pos.y;

			//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data. possibly other checks also.)
			if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex].getRemoteAddress() == connectedNetworkManagers[i].getRemoteAddress())) {
				std::cerr << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ") tried to send a message to an invalid NetworkManager (ip: "
					<< connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << ", " << connectedNetworkManagers[networkManagerIndex].getRemotePort() << ")!" << std::endl;
				continue;
			}

			//Send data to NetworkManager
			std::cout << "\n\n( " << connectedNetworkManagers[i].getRemoteAddress() << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << "\n\n";
			connectedNetworkManagers[networkManagerIndex].send(outgoingData);

			break;
		}
		}
	}

	//NetworkManagerInfo incomingNetworkManager {incomingNetworkManagerAddress, incomingNetworkManagerPort};

	//std::underlying_type_t<PacketCode> packetCode;
	//incomingData >> packetCode;
	//switch (packetCode)
	//{
	//case PacketCode::AddNetworkManager:
	//{
	//	std::cout << "PacketCode: AddNetworkManager\n";
	//	//Double check that NetworkManager isn't already in vector
	//	if (std::find(connectedNetworkManagers.begin(), connectedNetworkManagers.end(), incomingNetworkManager) != connectedNetworkManagers.end()) { return; }
	//	connectedNetworkManagers.push_back(incomingNetworkManager);
	//	std::cout << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") connected to server.\n";

	//	//Send NetworkManager index back to NetworkManager
	//	sf::Packet data;
	//	data << (int)connectedNetworkManagers.size() - 1;
	//	if (socket.send(data, incomingNetworkManager.ip, incomingNetworkManager.port) != sf::Socket::Done) { std::cerr << "Failed to send NetworkManager index to NetworkManager." << std::endl; }
	//	else { std::cerr << "Successfully sent NetworkManager index to NetworkManager.\n"; }

	//	return;
	//}

	//case PacketCode::RemoveNetworkManager:
	//{
	//	std::cout << "PacketCode: RemoveNetworkManager\n";
	//	std::cout << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") disconnected from server.\n";
	//	connectedNetworkManagers.erase(std::find(connectedNetworkManagers.begin(), connectedNetworkManagers.end(), incomingNetworkManager));
	//	return;
	//}
	//case PacketCode::CrouchChange:
	//{
	//	std::cout << "PacketCode: CrouchChange\n";

	//	//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
	//	int networkManagerIndex;
	//	int networkListenerIndex;
	//	bool crouched;
	//	incomingData >> networkManagerIndex >> networkListenerIndex >> crouched;
	//	std::cout << crouched << '\n';

	//	//Add networkListenerIndex, packetCode, and data to outgoingData
	//	sf::Packet outgoingData;
	//	outgoingData << networkListenerIndex << packetCode << crouched;

	//	//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data. possibly other checks also.)
	//	if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex] == incomingNetworkManager)) {
	//		std::cerr << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") tried to send a message to an invalid NetworkManager (ip: "
	//			      << connectedNetworkManagers[networkManagerIndex].ip << ", " << connectedNetworkManagers[networkManagerIndex].port << ")!" << std::endl;
	//		return;
	//	}

	//	//Send data to NetworkManager
	//	std::cout << "\n\n( " << incomingNetworkManagerAddress << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].ip << "\n\n";
	//	socket.send(outgoingData, connectedNetworkManagers[networkManagerIndex].ip, connectedNetworkManagers[networkManagerIndex].port);

	//	return;
	//}
	//case PacketCode::KeyChange:
	//{
	//	std::cout << "PacketCode: KeyChange\n";

	//	//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
	//	int networkManagerIndex;
	//	int networkListenerIndex;
	//	int key;
	//	bool pressed;
	//	incomingData >> networkManagerIndex >> networkListenerIndex >> pressed >> key;

	//	//Add networkListenerIndex, packetCode, and data to outgoingData
	//	sf::Packet outgoingData;
	//	outgoingData << networkListenerIndex << packetCode << pressed << key;

	//	//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data. possibly other checks also.)
	//	if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex] == incomingNetworkManager)) {
	//		std::cerr << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") tried to send a message to an invalid NetworkManager (ip: "
	//			      << connectedNetworkManagers[networkManagerIndex].ip << ", " << connectedNetworkManagers[networkManagerIndex].port << ")!" << std::endl;
	//		return;
	//	}

	//	//Send data to NetworkManager
	//	std::cout << "\n\n( " << incomingNetworkManagerAddress << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].ip << "\n\n";
	//	socket.send(outgoingData, connectedNetworkManagers[networkManagerIndex].ip, connectedNetworkManagers[networkManagerIndex].port);

	//	return;
	//}
	//case PacketCode::Verification:
	//{
	//	std::cout << "PacketCode: Verification\n";

	//	//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
	//	int networkManagerIndex;
	//	int networkListenerIndex;
	//	sf::Vector2f pos;
	//	sf::Int16 health;
	//	bool activeLimbs[4];
	//	incomingData >> networkManagerIndex >> networkListenerIndex >> pos.x >> pos.y >> health >> activeLimbs[0] >> activeLimbs[1] >> activeLimbs[2] >> activeLimbs[3];

	//	//Add networkListenerIndex, packetCode, and data to outgoingData
	//	sf::Packet outgoingData;
	//	outgoingData << networkListenerIndex << packetCode << pos.x << pos.y << health << activeLimbs[0] << activeLimbs[1] << activeLimbs[2] << activeLimbs[3];

	//	//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data. possibly other checks also.)
	//	if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex] == incomingNetworkManager)) {
	//		std::cerr << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") tried to send a message to an invalid NetworkManager (ip: "
	//			      << connectedNetworkManagers[networkManagerIndex].ip << ", " << connectedNetworkManagers[networkManagerIndex].port << ")!" << std::endl;
	//		return;
	//	}

	//	//Send data to NetworkManager
	//	std::cout << "\n\n( " << incomingNetworkManagerAddress << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].ip << "\n\n";
	//	socket.send(outgoingData, connectedNetworkManagers[networkManagerIndex].ip, connectedNetworkManagers[networkManagerIndex].port);

	//	return;
	//}
	//case PacketCode::Nums:
	//{
	//	int networkManagerIndex;
	//	int num;
	//	incomingData >> networkManagerIndex >> num;
	//	std::cout << num << '\n';

	//	//Add networkListenerIndex, packetCode, and data to outgoingData
	//	sf::Packet outgoingData;
	//	outgoingData << packetCode << num;

	//	//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data. possibly other checks also.)
	//	if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex] == incomingNetworkManager)) {
	//		std::cerr << "NetworkManager (ip: " << incomingNetworkManager.ip << ", " << incomingNetworkManager.port << ") tried to send a message to an invalid NetworkManager (ip: "
	//			      << connectedNetworkManagers[networkManagerIndex].ip << ", " << connectedNetworkManagers[networkManagerIndex].port << ")!" << std::endl;
	//		return;
	//	}

	//	//Send data to NetworkManager
	//	std::cout << "\n\n( " << incomingNetworkManagerAddress << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].ip << "\n\n";
	//	socket.send(outgoingData, connectedNetworkManagers[networkManagerIndex].ip, connectedNetworkManagers[networkManagerIndex].port);

	//	return;
	//}



	//std::cout << "\n\nPacket code uninterpretable.\n\n";
	//}
}
