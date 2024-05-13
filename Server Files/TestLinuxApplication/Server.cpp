#include "Server.h"
#include "PacketCode.h"
#include <algorithm> //For std::find
#include <iostream>
#include <random>

#include <string>

#include "sqlite3.h"

Server::Server(sf::IpAddress _ip, unsigned short _port) {
	serverAddress = _ip;
	serverPort = _port;

	//Assign tcp listener to port
	tcpListener.setBlocking(true);
	while (tcpListener.listen(serverPort) != sf::Socket::Done) {}
	std::cout << "TCP successfully assigned to port.\n";
	tcpListener.setBlocking(false);

	//Bind udp socket to port
	udpSocket.setBlocking(true);
	while (udpSocket.bind(serverPort) != sf::Socket::Done) {}
	std::cout << "UDP successfully bound to port.\n";
	udpSocket.setBlocking(false);

	timeBetweenTimeoutCheckPackets = 5;


	minItemBoxCooldownTime = 5;
	maxItemBoxCooldownTime = 10;
	//Populating goodDrops and badDrops
	for (int indexCount{ 0 }, enumCount{ START_OF_GOOD_DROPS + 1 }; enumCount != END_OF_GOOD_DROPS; ++indexCount, ++enumCount) {
		goodDrops[indexCount] = static_cast<ItemDrop>(enumCount);
	}
	for (int indexCount{ 0 }, enumCount{ START_OF_BAD_DROPS + 1 }; enumCount != END_OF_BAD_DROPS; ++indexCount, ++enumCount) {
		badDrops[indexCount] = static_cast<ItemDrop>(enumCount);
	}
}


void Server::CheckForIncomingConnectionRequests() {

	//Create new socket at smallest nmi that is free, and check if there's a connection request
	
	//Get smallest nmi that is free (e.g.: if nmis 0,1,2,5,6,9,12 were taken - this should return 3)
	int smallestFreeNMI{ 0 };
	for (int i{ 1 }; i <= connectedNetworkManagers.size(); ++i) {
		if (connectedNetworkManagers.find(i) == connectedNetworkManagers.end()) {
			//Smallest free value found
			smallestFreeNMI = i;
			break;
		}
	}

	connectedNetworkManagers[smallestFreeNMI].setBlocking(false);
	if (tcpListener.accept(connectedNetworkManagers[smallestFreeNMI]) == sf::Socket::Done) {

		std::cout << "NetworkManager (ip: " << connectedNetworkManagers[smallestFreeNMI].getRemoteAddress() << ", " << connectedNetworkManagers[smallestFreeNMI].getRemotePort() << ") connected to server.\n";

		//Send network manager index back to network manager
		sf::Packet outgoingPacket;
		int networkManagerIndex{ smallestFreeNMI };
		outgoingPacket << networkManagerIndex;
		connectedNetworkManagers[smallestFreeNMI].send(outgoingPacket);

		timeUntilNextTimeoutCheckPacket[smallestFreeNMI] = timeBetweenTimeoutCheckPackets;
	}
	else {
		//No connection request, remove from map
		connectedNetworkManagers.erase(smallestFreeNMI);
	}
}


void Server::CheckForIncomingTCPData(float dt) {

	//Loop through all connected tcp sockets to see if data is being received
	for (std::map<int, sf::TcpSocket>::iterator it{ connectedNetworkManagers.begin() }; it != connectedNetworkManagers.end(); ++it) {
		int i{ it->first };

		//Update and check status of client's time-until-timeout packet
		timeUntilNextTimeoutCheckPacket[i] -= dt;
		if (timeUntilNextTimeoutCheckPacket[i] <= 0) {
			sf::Packet timeoutCheckPacket;
			timeoutCheckPacket << -1; //-1 = discarded by client
			connectedNetworkManagers[i].setBlocking(true);
			if (connectedNetworkManagers[i].send(timeoutCheckPacket) != sf::Socket::Done) {
				//Failed to send timeout packet, disconnect user
				std::cout << "Failed to send timeout packet. Disconnecting client at nmi " << i << '\n';
				it = DisconnectUser(i);
				if (it != connectedNetworkManagers.end()) { --it; continue; } //Go back to previous value so that ++it will work correctly and not skip a value
				else { return; }
			}
			else {
				//Successfully sent timeout packet, reset timer
				timeUntilNextTimeoutCheckPacket[i] = timeBetweenTimeoutCheckPackets;
				connectedNetworkManagers[i].setBlocking(false);
			}
		}

		sf::Packet incomingData;
		if (connectedNetworkManagers[i].receive(incomingData) != sf::Socket::Done) { continue; } //No data being received from this socket, continue to the next one

		std::cout << "TCP Packet Received\n";

		std::underlying_type_t<PacketCode> packetCode;
		incomingData >> packetCode;
		switch (packetCode)
		{
		case PacketCode::RemoveNetworkManager:
		{
			std::cout << "PacketCode: RemoveNetworkManager\n";
			std::cout << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ", " << i << ") disconnected from server.\n";
			it = DisconnectUser(i);
			if (it != connectedNetworkManagers.end()) { --it; continue; } //Go back to previous value so that ++it will work correctly and not skip a value
			else { return; }
		}
		case PacketCode::UsernameRegister:
		{
			std::cout << "PacketCode: UsernameRegister\n";
			int networkListenerIndex;
			std::string username;
			incomingData >> networkListenerIndex >> username;

			sf::Int8 usernameAvailability{ -1 };
			usernameAvailability = AccountExists(username) ? 1 : 0;
			sf::Packet outgoingData;
			packetCode = PacketCode::UsernameAvailabilityStatus;
			outgoingData << networkListenerIndex << packetCode << usernameAvailability;

			std::cout << "This packet is being sent to network manager at ip " << connectedNetworkManagers[i].getRemoteAddress() << '\n';
			connectedNetworkManagers[i].send(outgoingData);

			if (usernameAvailability != 0) {
				std::cout << "Username " << username << " is unavailable.\n";
				continue;
			}


			//----PART 2: GENERATE UNIQUE RANDOM UUID----//
			//(note on collision: https://lemire.me/blog/2019/12/12/are-64-bit-random-identifiers-free-from-collision/)

			std::cout << "PacketCode: UUID\n";

			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<uint64_t> dis;

			sf::Uint64 uuid{ dis(gen) };
			std::cout << "UUID: " << uuid << '\n';

			//Open database
			sqlite3* db;
			sqlite3_open("LimbForLimbDatabase.db", &db);

			//Add username + uuid to account info table
			sqlite3_exec(db, "START TRANSACTION;", NULL, NULL, NULL);
			std::string aiInsertQuery{ "INSERT INTO AccountInfo (Username, UUID) VALUES (?, ?);"};
			sqlite3_stmt* aiInsertStmt;
			
			//Compile
			if (sqlite3_prepare_v2(db, aiInsertQuery.c_str(), -1, &aiInsertStmt, NULL) != SQLITE_OK) {
				std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;

				//Dont forget to close the database
				sqlite3_close(db);

				continue;
			}

			//Bind params
			sqlite3_bind_text(aiInsertStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(aiInsertStmt, 2, std::to_string(uuid).c_str(), -1, SQLITE_TRANSIENT);

			//Execute statement
			if (sqlite3_step(aiInsertStmt) != SQLITE_DONE) {
				std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;

				//Dont forget to close the database
				sqlite3_close(db);

				continue;
			}

			std::cout << "User " << username << " with UUID " << uuid << " was successfully added to the account info table.\n";

			sqlite3_finalize(aiInsertStmt);
			sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);



			//----PART 3: ADD ACCOUNT TO ACCOUNT RANKING TABLE----//
			
			sf::Int32 ranking{ 1000 }; //Starting ranking

			sqlite3_exec(db, "START TRANSACTION;", NULL, NULL, NULL);
			std::string arInsertQuery{ "INSERT INTO AccountRanking (Username, Ranking) VALUES (?, ?)" };
			sqlite3_stmt* arInsertStmt;

			//Compile
			if (sqlite3_prepare_v2(db, arInsertQuery.c_str(), -1, &arInsertStmt, NULL) != SQLITE_OK) {
				std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;

				//Dont forget to close the database
				sqlite3_close(db);

				continue;
			}

			//Bind params
			sqlite3_bind_text(arInsertStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(arInsertStmt, 2, std::to_string(ranking).c_str(), -1, SQLITE_TRANSIENT);

			//Execute statement
			if (sqlite3_step(arInsertStmt) != SQLITE_DONE) {
				std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;

				//Dont forget to close the database
				sqlite3_close(db);

				continue;
			}

			std::cout << "User " << username << " with ranking " << ranking << " was successfully added to the account ranking table.\n";

			sqlite3_finalize(arInsertStmt);
			sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);

			//Dont forget to close the database
			sqlite3_close(db);



			//----PART 4: SEND BACK UUID TO CLIENT----//
			{
				sf::Packet outgoingData;
				packetCode = PacketCode::UUID;
				outgoingData << networkListenerIndex << packetCode << uuid;
				
				//Send data to NetworkManager
				std::cout << "This packet is being sent to network manager at ip " << connectedNetworkManagers[i].getRemoteAddress() << '\n';
				connectedNetworkManagers[i].send(outgoingData);
			}



			//----PART 5: ADD USER TO MAPS----//
			onlineUsers[i] = username;
			onlineUserRankings[username] = ranking;
			//--------------------------------//

			break;
		}
		case PacketCode::Login:
		{
			std::cout << "PacketCode: Login\n";
			int networkListenerIndex;
			std::string username;
			sf::Uint64 uuid;

			incomingData >> networkListenerIndex >> username >> uuid;

			//Open database
			sqlite3* db;
			sqlite3_open("LimbForLimbDatabase.db", &db);



			//----PART 1: CHECK IF USERNAME EXISTS IN ACCOUNT INFO TABLE----//

			std::string credentialsCorrectQuery{ "SELECT EXISTS(SELECT 1 FROM AccountInfo WHERE Username = ? AND UUID = ?);" };
			//std::string credentialsCorrectQuery{ "SELECT * FROM AccountInfo;" };

			//Compile statement
			sqlite3_stmt* credentialsCorrectStmt;
			if (sqlite3_prepare_v2(db, credentialsCorrectQuery.c_str(), -1, &credentialsCorrectStmt, NULL) != SQLITE_OK) {
				std::cerr << "Error preparing statement:" << sqlite3_errmsg(db) << std::endl;
				//Dont forget to close the database
				sqlite3_close(db);
				continue;
			}

			//Bind username and uuid to statement
			sqlite3_bind_text(credentialsCorrectStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(credentialsCorrectStmt, 2, std::to_string(uuid).c_str(), -1, SQLITE_TRANSIENT);

			//Run statement
			int credentialsCorrectResult{ sqlite3_step(credentialsCorrectStmt) };

			//Send username availability back to network manager
			sf::Int8 credentialsCorrect{ -1 };
			if (credentialsCorrectResult == SQLITE_ROW) {
				credentialsCorrect = sqlite3_column_int(credentialsCorrectStmt, 0); //0 if credentials don't exist, 1 if credentials do exist
			}
			else {
				//Error
				std::cerr << "Unable to parse result, result value is: " << credentialsCorrectResult << std::endl;
				continue;
			}
			std::cout << "Credentials correct is: " << static_cast<int>(credentialsCorrect) << '\n';

			{
				//Add networkListenerIndex, packetCode, and data to outgoingData
				sf::Packet outgoingData;
				packetCode = PacketCode::LoginStatus;
				outgoingData << networkListenerIndex << packetCode << credentialsCorrect;

				//Send data to NetworkManager
				std::cout << "This packet is being sent to network manager at ip " << connectedNetworkManagers[i].getRemoteAddress() << '\n';
				connectedNetworkManagers[i].send(outgoingData);
			}
			sqlite3_finalize(credentialsCorrectStmt);


			if (credentialsCorrect != 1) {

				std::cout << "Matching credentials not found.\n";

				//Dont forget to close the database
				sqlite3_close(db);
				continue;
			}



			//----PART 2: SEND RANKING TO CLIENT----//
			
			std::string rankingQuery{ "SELECT Ranking FROM AccountRanking WHERE Username = ?;" };

			//Compile statement
			sqlite3_stmt* rankingStmt;
			if (sqlite3_prepare_v2(db, rankingQuery.c_str(), -1, &rankingStmt, NULL) != SQLITE_OK) {
				std::cerr << "Error preparing statement:" << sqlite3_errmsg(db) << std::endl;
				//Dont forget to close the database
				sqlite3_close(db);
				continue;
			}

			//Bind username to statement
			sqlite3_bind_text(rankingStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

			//Run statement
			sqlite3_step(rankingStmt);

			//Send ranking back to network manager
			sf::Int32 ranking{ sqlite3_column_int(rankingStmt, 0) };

			{
				//Add networkListenerIndex, packetCode, and data to outgoingData
				sf::Packet outgoingData;
				packetCode = PacketCode::Ranking;
				outgoingData << networkListenerIndex << packetCode << ranking;

				//Send data to NetworkManager
				std::cout << "This packet is being sent to network manager at ip " << connectedNetworkManagers[i].getRemoteAddress() << '\n';
				connectedNetworkManagers[i].send(outgoingData);
			}

			sqlite3_finalize(rankingStmt);
			
			//Dont forget to close the database
			sqlite3_close(db);

			//--------------------------------------------------//



			//----PART 3: ADD USER TO MAPS----//
			onlineUsers[i] = username;
			onlineUserRankings[username] = ranking;

			//---------------------------------------//


			break;
		}
		case PacketCode::Logout:
		{
			std::cout << "PacketCode: Logout\n";
			//Log out user that sent the packet
			onlineUserRankings.erase(onlineUsers[i]);
			onlineUsers.erase(i);

			break;
		}
		case PacketCode::UsernameInvite:
		{

			std::cout << "PacketCode: UsernameInvite\n";

			int networkListenerIndex;
			std::string username;
			incomingData >> networkListenerIndex >> username;

			//Send user-exists packet
			{
				std::cout << "PacketCode: UserExists\n";
				sf::Packet outgoingData;
				packetCode = PacketCode::UserExists;
				sf::Int8 userExists{ static_cast<sf::Int8>(AccountExists(username) ? 0 : 1) };
				outgoingData << networkListenerIndex << packetCode << userExists;
				connectedNetworkManagers[i].send(outgoingData);
			}

			//Loop through all online users to find a match
			int invitedNetworkMangerIndex{ -1 };
			for (std::map<int, std::string>::iterator it{ onlineUsers.begin() }; it != onlineUsers.end(); ++it) {
				if (it->second == username) {
					invitedNetworkMangerIndex = it->first;
					break;
				}
			}

			//Send user-online packet
			{
				std::cout << "PacketCode: UserOnline\n";
				sf::Packet outgoingData;
				packetCode = PacketCode::UserOnline;
				sf::Int8 userOnline{ static_cast<sf::Int8>((invitedNetworkMangerIndex == -1) ? 1 : 0) };
				outgoingData << networkListenerIndex << packetCode << userOnline;
				connectedNetworkManagers[i].send(outgoingData);
			}

			if (invitedNetworkMangerIndex == -1) {
				continue;
			}

			//Send network manager index of invited client back to inviting user
			{
				std::cout << "PacketCode: InvitedUserNetworkManagerIndex\n";
				sf::Packet outgoingData;
				packetCode = PacketCode::InvitedUserNetworkManagerIndex;
				outgoingData << networkListenerIndex << packetCode << invitedNetworkMangerIndex;
				connectedNetworkManagers[i].send(outgoingData);
			}

			//Loop through all matched users to see if invited user is currently in a match
			bool invitedUserInMatch{ false };
			for (std::map<int, int>::iterator it{ matchedUsers.begin() }; it != matchedUsers.end(); ++it) {
				if (invitedNetworkMangerIndex == it->first || invitedNetworkMangerIndex == it->second) {
					invitedUserInMatch = true;
				}
			}

			//Send user-free packet
			{
				std::cout << "PacketCode: UserFree\n";
				sf::Packet outgoingData;
				packetCode = PacketCode::UserFree;
				sf::Int8 userFree{ static_cast<sf::Int8>(invitedUserInMatch ? 1 : 0) };
				outgoingData << networkListenerIndex << packetCode << userFree;
				connectedNetworkManagers[i].send(outgoingData);
			}

			if (invitedUserInMatch) {
				continue;
			}


			//User exists, is online, and isn't currently in a match - they are ready to be invited. Send inviting user's username, ranking, and nmi
			{
				std::cout << "PacketCode: MatchInvitation\n";
				sf::Packet outgoingData;
				packetCode = PacketCode::MatchInvitation;
				std::string invitingClientUsername{ onlineUsers[i] }; //Username of the client that's sending the invitation
				sf::Int32 invitingClientRanking{ onlineUserRankings[invitingClientUsername] }; //Ranking of the client that's sending the invitation
				outgoingData << ReservedEntityIndexTable::MATCH_INVITATION_INTERRUPT << packetCode << invitingClientUsername << invitingClientRanking << i;
				connectedNetworkManagers[invitedNetworkMangerIndex].send(outgoingData);
			}


			break;
		}
		case PacketCode::MatchAcceptanceClientToServer:
		{
			std::cout << "PacketCode: MatchAcceptanceClientToServer\n";
			int networkListenerIndex;
			sf::Int8 acceptance;
			int opponentNetworkManagerIndex;

			incomingData >> networkListenerIndex >> acceptance >> opponentNetworkManagerIndex;

			//Send acceptance status to inviting client
			{
				std::cout << "PacketCode: MatchAcceptanceServerToClient\n";
				sf::Packet outgoingData;
				packetCode = PacketCode::MatchAcceptanceServerToClient;
				outgoingData << ReservedEntityIndexTable::SEND_INVITE_SCREEN << packetCode << acceptance;
				connectedNetworkManagers[opponentNetworkManagerIndex].send(outgoingData);
			}

			if (acceptance == 0) {
				//User has accepted match invitation, send both players their player numbers
				sf::Int8 invitingClientPlayerNum{ static_cast<sf::Int8>((rand() % 2) + 1)};
				std::cout << "Inviting player num: " << static_cast<int>(invitingClientPlayerNum) << '\n';
				{
					std::cout << "PacketCode: PlayerNum\n";
					sf::Packet outgoingData;
					packetCode = PacketCode::PlayerNum;
					outgoingData << ReservedEntityIndexTable::SEND_INVITE_SCREEN << packetCode << invitingClientPlayerNum;
					connectedNetworkManagers[opponentNetworkManagerIndex].send(outgoingData);
				}
				sf::Int8 invitedClientPlayerNum{ static_cast<sf::Int8>((invitingClientPlayerNum == 1) ? 2 : 1) };
				std::cout << "Invited player num: " << static_cast<int>(invitedClientPlayerNum) << '\n';
				{
					std::cout << "PacketCode: PlayerNum\n";
					sf::Packet outgoingData;
					packetCode = PacketCode::PlayerNum;
					outgoingData << networkListenerIndex << packetCode << invitedClientPlayerNum;
					connectedNetworkManagers[i].send(outgoingData);
				}

				//Add players to match maps
				matchedUsers[i] = opponentNetworkManagerIndex;

				//Get smallest match index that is free (e.g.: if match indices 0,1,2,5,6,9,12 were taken - this should return 3)
				int smallestFreeIndex{ 0 };
				for (int i{ 1 }; i <= matches.size(); ++i) {
					if (matches.find(i) == matches.end()) {
						//Smallest free value found
						smallestFreeIndex = i;
						break;
					}
				}

				matches[smallestFreeIndex] = std::make_pair(i, opponentNetworkManagerIndex);
				timeUntilNextItemBoxInMatch[smallestFreeIndex] = rand() % static_cast<int>(maxItemBoxCooldownTime - minItemBoxCooldownTime + 1) + minItemBoxCooldownTime;
			}

			break;
		}
		case PacketCode::MatchSceneLoaded:
		{

			std::cout << "PacketCode: MatchSceneLoaded\n";
			bool loaded;
			incomingData >> loaded;

			//Add loaded state to userMatchSceneLoaded
			userMatchSceneLoaded[i] = loaded;

			//Check if both users' match scenes have finished loading
			int opponentNMI{ GetOpponentNMI(i) };
			if (opponentNMI != -1) {
				//Opponent user found
				if (userMatchSceneLoaded[i] && userMatchSceneLoaded[opponentNMI]) {
					//Both scenes have loaded
					std::cout << "PacketCode: MatchStart\n";
					sf::Packet outgoingData;
					int networkListenerIndex{ ReservedEntityIndexTable::NETWORK_SCENE };
					packetCode = PacketCode::MatchStart;
					outgoingData << networkListenerIndex << packetCode;
					connectedNetworkManagers[i].send(outgoingData);
					connectedNetworkManagers[opponentNMI].send(outgoingData);

					//Erase entries from userMatchSceneLoaded map
					userMatchSceneLoaded.erase(i);
					userMatchSceneLoaded.erase(opponentNMI);
				}
			}
			else {
				std::cerr << "Failed to find opponent of nmi " << i << std::endl;
			}

			break;
		}
		case PacketCode::MatchWin:
		{
			std::cout << "PacketCode: MatchWin\n";
			std::cout << "Winning player: " << onlineUsers[i];
			AwardMatchWin(i);

			//Remove both players from matchedUsers map
			for (std::map<int, int>::iterator it{ matchedUsers.begin() }; it != matchedUsers.end(); ++it) {
				if ((it->first == i) || (it->second == i)) {
					matchedUsers.erase(it);
					break;
				}
			}

			//Remove entries from matches map and timeUntilNextItemBoxInMatch map
			for (std::map<int, std::pair<int, int>>::iterator it{ matches.begin() }; it != matches.end(); ++it) {
				if ((it->second.first == i) || (it->second.second == i)) {
					matches.erase(it);
					timeUntilNextItemBoxInMatch.erase(it->first);
					break;
				}
			}

			break;
		}
		case PacketCode::MatchLeave:
		{
			std::cout << "PacketCode: MatchLeave\n";

			int opponentNetworkManagerIndex{ GetOpponentNMI(i) };

			std::cout << "Winning player: " << onlineUsers[opponentNetworkManagerIndex];
			AwardMatchWin(opponentNetworkManagerIndex);

			//Remove both players from matchedUsers map
			for (std::map<int, int>::iterator it{ matchedUsers.begin() }; it != matchedUsers.end(); ++it) {
				if ((it->first == i) || (it->second == i)) {
					matchedUsers.erase(it);
					break;
				}
			}

			//Remove entry from matches map and timeUntilNextItemBoxInMatch map
			for (std::map<int, std::pair<int, int>>::iterator it{ matches.begin() }; it != matches.end(); ++it) {
				if ((it->second.first == i) || (it->second.second == i)) {
					matches.erase(it);
					timeUntilNextItemBoxInMatch.erase(it->first);
					break;
				}
			}

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
				std::cerr << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ") tried to send a message to an invalid NetworkManager!" << std::endl;
				continue;
			}

			//Send data to NetworkManager
			std::cout << "( " << connectedNetworkManagers[i].getRemoteAddress() << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << '\n';
			connectedNetworkManagers[networkManagerIndex].send(outgoingData);

			break;
		}
		case PacketCode::Grounded:
		{
			std::cout << "PacketCode: Grounded\n";

			//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
			int networkManagerIndex;
			int networkListenerIndex;
			bool grounded;
			incomingData >> networkManagerIndex >> networkListenerIndex >> grounded;

			//Add networkListenerIndex, packetCode, and data to outgoingData
			sf::Packet outgoingData;
			outgoingData << networkListenerIndex << packetCode << grounded;

			//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data)
			if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex].getRemoteAddress() == connectedNetworkManagers[i].getRemoteAddress())) {
				std::cerr << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ") tried to send a message to an invalid NetworkManager!" << std::endl;
				continue;
			}

			//Send data to NetworkManager
			std::cout << "( " << connectedNetworkManagers[i].getRemoteAddress() << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << '\n';
			connectedNetworkManagers[networkManagerIndex].send(outgoingData);

			break;
		}
		case PacketCode::Flip:
		{
			std::cout << "PacketCode: Flip\n";

			//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
			int networkManagerIndex;
			int networkListenerIndex;
			bool flipped;
			incomingData >> networkManagerIndex >> networkListenerIndex >> flipped;

			//Add networkListenerIndex, packetCode, and data to outgoingData
			sf::Packet outgoingData;
			outgoingData << networkListenerIndex << packetCode << flipped;

			//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data)
			if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex].getRemoteAddress() == connectedNetworkManagers[i].getRemoteAddress())) {
				std::cerr << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ") tried to send a message to an invalid NetworkManager!" << std::endl;
				continue;
			}

			//Send data to NetworkManager
			std::cout << "( " << connectedNetworkManagers[i].getRemoteAddress() << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << '\n';
			connectedNetworkManagers[networkManagerIndex].send(outgoingData);

			break;
		}
		case PacketCode::Health:
		{
			std::cout << "PacketCode: Health\n";

			//Separate packet, networkManagerIndex, and networkListenerIndex from incoming data
			int networkManagerIndex;
			int networkListenerIndex;
			int health;
			incomingData >> networkManagerIndex >> networkListenerIndex >> health;

			//Add networkListenerIndex, packetCode, and data to outgoingData
			sf::Packet outgoingData;
			outgoingData << networkListenerIndex << packetCode << health;

			//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data)
			if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex].getRemoteAddress() == connectedNetworkManagers[i].getRemoteAddress())) {
				std::cerr << "NetworkManager (ip: " << connectedNetworkManagers[i].getRemoteAddress() << ", " << connectedNetworkManagers[i].getRemotePort() << ") tried to send a message to an invalid NetworkManager!" << std::endl;
				continue;
			}

			//Send data to NetworkManager
			std::cout << "( " << connectedNetworkManagers[i].getRemoteAddress() << "): This packet is being sent to network manager at ip " << connectedNetworkManagers[networkManagerIndex].getRemoteAddress() << '\n';
			connectedNetworkManagers[networkManagerIndex].send(outgoingData);

			break;
		}
		default:
		{
			std::cerr << "Packet code uninterpretable." << std::endl;
			return;
		}
		}
	}
}


void Server::CheckForIncomingUDPData() {
	sf::Packet incomingData;
	sf::IpAddress incomingNetworkManagerAddress;
	unsigned short incomingNetworkManagerPort;

	if (udpSocket.receive(incomingData, incomingNetworkManagerAddress, incomingNetworkManagerPort) != sf::Socket::Done) { return; }

	std::cout << "UDP Packet Received\n";

	std::underlying_type_t<PacketCode> packetCode;
	incomingData >> packetCode;
	switch (packetCode)
	{
	case PacketCode::UDPConnect:
	{
		std::cout << "PacketCode: UDPConnect\n";
		int networkManagerIndex;
		incomingData >> networkManagerIndex;
		connectedUdpPorts[networkManagerIndex] = incomingNetworkManagerPort;
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

		//Add networkListenerIndex, packetCode, and data to outgoingData
		sf::Packet outgoingData;
		outgoingData << networkListenerIndex << packetCode << pos.x << pos.y;

		//Validate data (make sure NetworkManager is trying to send data to an ip+port that is in the array and make sure NetworkManager isn't trying to send themselves data. possibly other checks also.)
		if ((networkManagerIndex >= connectedNetworkManagers.size()) || (connectedNetworkManagers[networkManagerIndex].getRemoteAddress() == incomingNetworkManagerAddress)) {
			std::cerr << "NetworkManager (ip: " << incomingNetworkManagerAddress << ", " << incomingNetworkManagerPort << ") tried to send a message to an invalid NetworkManager!" << std::endl;
			std::cerr << "NETWORK MANAGER INDEX IS: " << networkManagerIndex << '\n';
			break;
		}

		//Send data to NetworkManager
		udpSocket.send(outgoingData, connectedNetworkManagers[networkManagerIndex].getRemoteAddress(), connectedUdpPorts[networkManagerIndex]);

		break;
	}
	default:
	{
		std::cerr << "Packet code uninterpretable." << std::endl;
		return;
	}
	}
}


void Server::UpdateAndCheckItemBoxCooldowns(float dt)
{
	for (std::map<int, float>::iterator it{ timeUntilNextItemBoxInMatch.begin() }; it != timeUntilNextItemBoxInMatch.end(); ++it) {
		it->second -= dt;
		if (it->second <= 0) {
			//Ready to spawn an item box, reset the cooldown and send an ItemBox packet to both clients

			it->second = rand() % static_cast<int>(maxItemBoxCooldownTime - minItemBoxCooldownTime + 1) + minItemBoxCooldownTime;

			sf::Packet outgoingPacket;
			int randXPos{ rand() % (1920 - 30) }; //30 is the width of the item box

			float chanceOfBeingGood = 0.65f;
			int riskRewardMultiplier{ 2 }; //Scale the risk-reward by the multiplier
			float randVal{ static_cast<float>(rand()) / RAND_MAX };
			float riskReward{ randVal * riskRewardMultiplier };

			//Determine if good or bad and choose random drop to assign to item box
			randVal = static_cast<float>(rand()) / RAND_MAX;
			ItemDrop drop = (randVal <= chanceOfBeingGood) ? (goodDrops[rand() % sizeof(goodDrops) / sizeof(goodDrops[0])]) : (badDrops[rand() % sizeof(badDrops) / sizeof(badDrops[0])]);

			outgoingPacket << static_cast<std::underlying_type_t<ReservedEntityIndexTable>>(ReservedEntityIndexTable::NETWORK_SCENE);
			outgoingPacket << static_cast<std::underlying_type_t<PacketCode>>(PacketCode::ItemBoxSpawn);
			outgoingPacket << randXPos;
			outgoingPacket << static_cast<std::underlying_type_t<ItemDrop>>(drop);
			outgoingPacket << riskReward;

			connectedNetworkManagers[matches[it->first].first].send(outgoingPacket);
			connectedNetworkManagers[matches[it->first].second].send(outgoingPacket);
		}
	}
}



std::map<int, sf::TcpSocket>::iterator Server::DisconnectUser(int nmi)
{
	//Check if user is logged in
	if (onlineUsers.find(nmi) != onlineUsers.end()) {
		//Check if user is in match - if so, end the match and award a win to their opponent
		int opponentNMI{ GetOpponentNMI(nmi) };
		if (opponentNMI != -1) {
			//User in match - award win to opponent
			AwardMatchWin(opponentNMI);

			//Remove both players from matchedUsers map
			for (std::map<int, int>::iterator it{ matchedUsers.begin() }; it != matchedUsers.end(); ++it) {
				if ((it->first == nmi) || (it->second == nmi)) {
					matchedUsers.erase(it);
					break;
				}
			}

			//Remove entry from matches map and timeUntilNextItemBoxInMatch map
			for (std::map<int, std::pair<int, int>>::iterator it{ matches.begin() }; it != matches.end(); ++it) {
				if ((it->second.first == nmi) || (it->second.second == nmi)) {
					matches.erase(it);
					timeUntilNextItemBoxInMatch.erase(it->first);
					break;
				}
			}
		}

		//User is logged in, log them out
		onlineUserRankings.erase(onlineUsers[nmi]);
		onlineUsers.erase(nmi);

	}

	std::map<int, sf::TcpSocket>::iterator iterator{ connectedNetworkManagers.erase(connectedNetworkManagers.find(nmi)) };
	connectedUdpPorts.erase(nmi);
	timeUntilNextTimeoutCheckPacket.erase(nmi);

	return iterator;
}



bool Server::AccountExists(std::string username)
{
	//Open database
	sqlite3* db;
	sqlite3_open("LimbForLimbDatabase.db", &db);

	std::string usernameExistsQuery{ "SELECT EXISTS(SELECT 1 FROM AccountInfo WHERE Username = ?);" };

	//Compile statement
	sqlite3_stmt* usernameExistsStmt;
	if (sqlite3_prepare_v2(db, usernameExistsQuery.c_str(), -1, &usernameExistsStmt, NULL) != SQLITE_OK) {
		std::cerr << "Error preparing statement:" << sqlite3_errmsg(db) << std::endl;
		//Dont forget to close the database
		sqlite3_close(db);
		exit(-1);
	}

	//Bind username to statement
	sqlite3_bind_text(usernameExistsStmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

	//Run statement
	int result = sqlite3_step(usernameExistsStmt);

	bool usernameExists;
	if (result == SQLITE_ROW) {
		usernameExists = sqlite3_column_int(usernameExistsStmt, 0); //0 if username doesn't exist, 1 if username does exist
	}
	else {
		//Error
		std::cerr << "Unable to parse result, result value is: " << result << std::endl;
	}

	sqlite3_finalize(usernameExistsStmt);
	sqlite3_close(db);
	return usernameExists;
}



int Server::GetOpponentNMI(int nmi)
{
	for (std::map<int, int>::iterator it{ matchedUsers.begin() }; it != matchedUsers.end(); ++it) {
		if (it->first == nmi) {
			return it->second;
		}
		else if (it->second == nmi) {
			return it->first;
		}
	}
	//User not found
	return -1;
}



void Server::AwardMatchWin(int winningNMI)
{
	int losingNMI{ GetOpponentNMI(winningNMI) };

	//+30 rank to winner and -30 rank to loser
	onlineUserRankings[onlineUsers[winningNMI]] += 30;
	onlineUserRankings[onlineUsers[losingNMI]] -= 30;

	//Loop through both players to update ranking in database and send MatchEnd packet
	//Open database
	sqlite3* db;
	sqlite3_open("LimbForLimbDatabase.db", &db);
	int nmis[2]{ winningNMI, losingNMI };
	for (int nmi : nmis) {
		
		std::string rankUpdateQuery{ "UPDATE AccountRanking SET Ranking = ? WHERE Username = ?" };

		//Compile statement
		sqlite3_stmt* rankUpdateStmt;
		if (sqlite3_prepare_v2(db, rankUpdateQuery.c_str(), -1, &rankUpdateStmt, NULL) != SQLITE_OK) {
			std::cerr << "Error preparing statement:" << sqlite3_errmsg(db) << std::endl;
			//Dont forget to close the database
			sqlite3_close(db);
			exit(-1);
		}

		//Bind ranking and username to statement
		sqlite3_bind_int(rankUpdateStmt, 1, onlineUserRankings[onlineUsers[nmi]]);
		sqlite3_bind_text(rankUpdateStmt, 2, onlineUsers[nmi].c_str(), -1, SQLITE_TRANSIENT);

		std::cout << "RANKING: " << onlineUserRankings[onlineUsers[nmi]] << ' ' << "USERNAME: " << onlineUsers[nmi].c_str() << '\n';

		//Run statement
		int result = sqlite3_step(rankUpdateStmt);

		if (result == SQLITE_DONE) {
			std::cout << "Successfully updated player's ranking in database\n.";

		}
		else {
			//Error
			std::cerr << "Unable to parse result, result value is: " << result << std::endl;
		}

		sqlite3_finalize(rankUpdateStmt);


		//Send MatchEnd packet to client
		sf::Packet outgoingPacket;
		outgoingPacket << ReservedEntityIndexTable::NETWORK_SCENE << PacketCode::MatchEnd << winningNMI << onlineUserRankings[onlineUsers[nmi]];
		connectedNetworkManagers[nmi].send(outgoingPacket);
	}

	//Don't forget to close the database... -_-
	sqlite3_close(db);
}