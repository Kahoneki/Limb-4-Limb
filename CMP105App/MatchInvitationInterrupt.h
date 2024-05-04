#ifndef MATCH_INVITATION_INTERRUPT_H
#define MATCH_INVITATION_INTERRUPT_H

#include <string>
#include "SFML/Graphics.hpp"

template<typename ParentType>
class NetworkListener;


//Handles any incoming match invitations
//A reference to this class can be added to any class that the user should be able to receive match invitations on (e.g. OnlineSelectScreen).
//The class that this is attached to should check `getInvitationReceived()` every frame to see if a match invitation has been received, and (for example), create a pop up if it returns true.

//This class exists because a user should be able to receive an invitation on multiple different scenes, so the inviting user / server cannot know the network listener index of the scene that the
//receiving user is currently on to be able to send a packet to that network listener. Instead, this class's network listener can hold a reserved spot which will be known to both clients and the server


class MatchInvitationInterrupt
{
public:
	static MatchInvitationInterrupt& getInstance();

	bool getInvitationReceived();
	std::string getUsername();
	sf::Int32 getRanking();
	int getNetworkManagerIndex();

	friend class NetworkListener<MatchInvitationInterrupt>;

private:
	MatchInvitationInterrupt();
	
	NetworkListener<MatchInvitationInterrupt>* networkListener;
	bool invitationReceived;

	//Information of the user that sent the invite - to be populated once invitation is received
	std::string username;
	sf::Int32 ranking;
	int networkManagerIndex;
};

#endif