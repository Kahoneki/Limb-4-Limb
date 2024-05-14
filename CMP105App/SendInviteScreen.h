#ifndef SEND_INVITE_SCREEN_H
#define SEND_INVITE_SCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>

#include "InputBox.h"
#include "Button.h"


//Forward declarations
class SceneManager;
class NetworkManager;
template<typename ParentType>
class NetworkListener;

class SendInviteScreen : public BaseLevel
{
public:

	SendInviteScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~SendInviteScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	friend class NetworkListener<SendInviteScreen>;


private:
	SceneManager& sceneManager;
	NetworkManager& networkManager;

	sf::Font font;
	sf::RectangleShape background;

	InputBox usernameBox;

	Button sendInviteButton;
	Button backButton;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onSendInviteButtonClick;
	std::function<void(void)> onBackButtonClick;

	TextBox statusBar; //For displaying status to user - e.g.: connecting to server, searching for user, awaiting response from user, etc.
	bool displayStatusBar;

	bool awaitServerResponses;
	NetworkListener<SendInviteScreen>* networkListener;
	int networkListenerIndex;
	sf::Int8 userExists; //-1 = server hasn't responded yet, 0 = user exists, 1 = user doesn't exist
	sf::Int8 userOnline; //-1 = server hasn't responded yet, 0 = user online, 1 = user offline
	sf::Int8 userFree;   //-1 = server hasn't responded yet, 0 = user free, 1 = user currently in match
	sf::Int8 userAccept; //-1 = user hasn't responded yet, 0 = user accepted, 1 = user declined
	int invitedUserNetworkManagerIndex; //The network manager index of the user that this client has sent an invitation to
	int playerNum; //Player num of this client
};

#endif