#ifndef PACKETCODE_H
#define PACKETCODE_H

//Used for encoding the type of data a packet contains
enum PacketCode
{
	AddNetworkManager,
	RemoveNetworkManager,

	KeyChange,

	Verification,
	Nums

	//PositionChange
	//CrouchChange,
	//HealthChange,
	//StunFrameChange,
	//ActiveLimbChange,
	//AttackingChange
};

#endif