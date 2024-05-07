#ifndef MATCH_INVITATION_INTERRUPT_H
#define MATCH_INVITATION_INTERRUPT_H

#include <string>
#include "TextBox.h"
#include "Button.h"
#include <functional>

template<typename ParentType>
class NetworkListener;


//Handles any incoming match invitations
//A reference to this class can be added to any class that the user should be able to receive match invitations on (e.g. OnlineSelectScreen).
//The class that this is attached to should check `getInvitationReceived()` every frame to see if a match invitation has been received, and (for example), create a pop up if it returns true.

//This class exists because a user should be able to receive an invitation on multiple different scenes, so the inviting user / server cannot know the network listener index of the scene that the
//receiving user is currently on to be able to send a packet to that network listener. Instead, this class's network listener can hold a reserved spot which will be known to both clients and the server


class MatchInvitationInterrupt : public GameObject
{
public:
	static MatchInvitationInterrupt& getInstance();

	void processEvents(sf::Vector2f mousePos);

	bool getInvitationReceived();
	int getNetworkManagerIndex();
	bool getStartMatch();
	bool getPlayerNum();
	//std::string getUsername();
	//sf::Int32 getRanking();

	friend class NetworkListener<MatchInvitationInterrupt>;

private:
	MatchInvitationInterrupt();
	
	sf::Font font;
	TextBox popupBox;
	Button acceptMatchInvitation;
	Button declineMatchInvitation;

	//Callbacks
	std::function<void(void)> onAcceptMatchInvitationButtonClick;
	std::function<void(void)> onDeclineMatchInvitationButtonClick;

	void InitialiseCallbacks();

	NetworkListener<MatchInvitationInterrupt>* networkListener;
	bool invitationReceived; //Value set by network
	bool valuesSet; //True if username and ranking field have been set

	//Information of the user that sent the invite - to be populated once invitation is received
	std::string username;
	sf::Int32 ranking;
	int networkManagerIndex; //Opponent's network manager index
	//----//

	int playerNum; //This player's player number (i.e. player 1 or player 2) - set by network

	bool startMatch; //Set true when player num is set by network

	//Override pure virtual draw function
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif