#ifndef PACKETCODE_H
#define PACKETCODE_H

//Used for encoding the type of data a packet contains
enum PacketCode
{

	//NETWORKING//
	UDPConnect,
	RemoveNetworkManager,
	//----//


	//GAMEPLAY//
	KeyChange,
	PositionChange,
	Grounded,
	Flip,
	Health,
	ItemBoxSpawn, //Holds x position, drop, and risk reward
	//----//


	//Debug//
	Nums,
	//----//


	//ACCOUNT//
	UsernameRegister, //From client
	UsernameAvailabilityStatus, //From server
	UUID, //From server
	Login, //From client
	LoginStatus, //From server
	Ranking, //From server
	Logout, //From client
	//----//


	//MATCHMAKING//
	UsernameInvite, //Stores username that client is sending an invite to
	UserExists, //Stores whether the entered username exists
	UserOnline, //Stores whether the entered username is online
	UserFree, //Stores whether the entered username is free to start a match (i.e. they're not already in a match)
	InvitedUserNetworkManagerIndex, //Stores the network manager of the user that the player has invited
	MatchInvitation, //Stores the match invitation information to be received by invited client (username + ranking + network manager index)
	MatchAcceptanceClientToServer, //Stores whether user accepts/denies match invitation and the nmi of the user that invited them - sending from client to server
	MatchAcceptanceServerToClient, //Stores whether user accepts/denies match invitation - sending from server to client
	PlayerNum, //Stores a player number (i.e.: player 1 or player 2)
	MatchSceneLoaded, //Sent by client when match scene is loaded - to make sure scene is loaded on both client's machine before match starts
	MatchStart, //Sent by server once both matched users have sent a positive-status MatchSceneLoaded packet to tell them to start the match
	MatchWin, //Sent by client if player wins - it is assumed that this means the other player has lost. This packet should only be sent by one of the two clients (the one which has won)
	MatchLeave, //Sent by client if player leaves (to main menu) - this will result in a loss for the player that has sent the packet and a win for their opponent
	MatchEnd, //Sent by server once match has ended (if client sends MatchWin packet or disconnects), holds winning player's nmi and client's new ranking
	//----//

};

#endif