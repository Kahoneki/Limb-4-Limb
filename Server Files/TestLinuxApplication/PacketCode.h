#ifndef PACKETCODE_H
#define PACKETCODE_H

//Used for encoding the type of data a packet contains
enum PacketCode
{
	AddNetworkManager,
	RemoveNetworkManager,

	KeyChange,

	Verification,
	PositionChange,
	Nums,

	Username, //From client
	UsernameAvailabilityStatus, //From server
	UUID, //From server

	Login, //From client
	LoginStatus, //From server
	Ranking, //From server
};

#endif