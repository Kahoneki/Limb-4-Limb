#ifndef PACKETCODE_H
#define PACKETCODE_H

//Used for encoding the type of data a packet contains
enum PacketCode
{
	AddNetworkManager,
	RemoveNetworkManager,

	KeyChange

	//PositionChange,
	//CrouchChange,
	//HealthChange,
	//StunFrameChange,
	//ActiveLimbChange,
	//AttackingChange
};

#endif