#ifndef RANDOM_MATCH_SCREEN_H
#define RANDOM_MATCH_SCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>

#include "InputBox.h"
#include "Button.h"


//Forward declarations
class SceneManager;

template<typename ParentType>
class NetworkListener;

class RandomMatchScreen : public BaseLevel
{
public:

	RandomMatchScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~RandomMatchScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	void setUserExists(sf::Int8 val);
	void setUserOnline(sf::Int8 val);
	void setUserAccept(sf::Int8 val);


private:
	SceneManager& sceneManager;

	sf::Font font;
	sf::RectangleShape background;

	InputBox usernameBox;

	Button sendInviteButton; //To confirm registration
	Button backButton;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onSendInviteButtonClick;
	std::function<void(void)> onBackButtonClick;

	TextBox statusBar; //For displaying status to user - e.g.: connecting to server, searching for user, awaiting response from user, etc.
	bool displayStatusBar;

	bool awaitServerResponses;
	void sendUsernameToServer();
	NetworkListener<RandomMatchScreen>* networkListener;
	int networkListenerIndex;
	sf::Int8 userExists; //-1 = server hasn't responded yet, 0 = user exists, 1 = user doesn't exist
	sf::Int8 userOnline; //-1 = server hasn't responded yet, 0 = user online, 1 = user doesn't exist
	sf::Int8 userAccept; //-1 = user hasn't responded yet, 0 = user accepted, 1 = user declined
};

#endif