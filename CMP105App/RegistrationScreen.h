#ifndef REGISTRATION_SCREEN_H
#define REGISTRATION_SCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>

#include "InputBox.h"

class SceneManager; //Forward declaration

template<typename ParentType>
class NetworkListener;

class RegistrationScreen : public BaseLevel
{
public:

	RegistrationScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~RegistrationScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	void setUsernameAvailable(bool val);
	void setUUID(sf::Uint64 val);

private:
	SceneManager& sceneManager;

	sf::Font font;
	sf::RectangleShape background;
	
	bool mousePressedLastFrame;

	InputBox usernameBox;
	TextBox registerButton; //To confirm registration
	TextBox backButton;
	TextBox statusBar; //For displaying status to user - e.g.: connecting to server, generating uuid, username invalid, etc.
	bool displayStatusBar;

	bool checkClientSideUsernameValidity();
	int minUsernameLength;
	int maxUsernameLength;

	bool awaitServerResponses;
	void sendUsernameToServer();
	NetworkListener<RegistrationScreen>* networkListener;
	int networkListenerIndex;
	sf::Int8 usernameAvailable; //-1 = server hasn't responded yet, 0 = username available, 1 = username not available
	sf::Uint64 uuid;
};

#endif