#ifndef PACKETCODE_H
#define PACKETCODE_H

//Used for encoding the type of data a packet contains
enum PacketCode
{
	RemoveNetworkManager,
	AccountRegistrationRequest,
	AccountLoginRequest,

	KeyChange,
	
	Verification,
	PositionChange,
	Nums
};

#endif
