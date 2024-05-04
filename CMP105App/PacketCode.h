#ifndef PACKETCODE_H
#define PACKETCODE_H

//Used for encoding the type of data a packet contains
enum PacketCode
{

	//NETWORKING//
	RemoveNetworkManager,
	OnlineCheck, //Periodically sent by server to make sure client is still online, if client receives this packet, they should send one back as a confirmation
	//----//


	//GAMEPLAY//
	KeyChange,
	PositionChange,
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
	//----//


	//MATCHMAKING//
	UsernameInvite, //Stores username that client is sending an invite to
	UserExists, //Stores whether the entered username exists
	UserOnline, //Stores whether the entered username is online
	UserFree, //Stores whether the entered username is free to start a match (i.e. they're not already in a match)
	InvitedUserNetworkManagerIndex, //Stores the network manager of the user that the player has invited
	MatchInvitation, //Stores the match invitation information to be received by invited client (username + ranking + network manager index)
	MatchAcceptance, //Stores true/false if user accepts/denies match invitation
	MatchSceneLoaded, //Sent when match scene is loaded - to make sure scene is loaded on both client's machine before match starts
	MatchWin, //Sent if player wins - it is assumed that this means the other player has lost. This packet should only be sent by one of the two clients (the one which has won)
	//----//

};

#endif