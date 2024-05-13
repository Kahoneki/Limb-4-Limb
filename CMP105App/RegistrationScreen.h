#ifndef REGISTRATION_SCREEN_H
#define REGISTRATION_SCREEN_H

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
class MatchInvitationInterrupt;

class RegistrationScreen : public BaseLevel
{
public:

	RegistrationScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm);
	~RegistrationScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	friend class NetworkListener<RegistrationScreen>;

private:
	SceneManager& sceneManager;
	NetworkManager& networkManager;

	sf::Font font;
	sf::RectangleShape background;

	InputBox usernameBox;

	Button registerButton; //To confirm registration
	Button backButton;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onRegisterButtonClick;
	std::function<void(void)> onBackButtonClick;

	TextBox statusBar; //For displaying status to user - e.g.: connecting to server, generating uuid, username invalid, etc.
	bool displayStatusBar;

	bool checkClientSideUsernameValidity();
	int minUsernameLength;
	int maxUsernameLength;

	bool awaitServerResponses;
	NetworkListener<RegistrationScreen>* networkListener;
	int networkListenerIndex;
	sf::Int8 usernameAvailable; //-1 = server hasn't responded yet, 0 = username available, 1 = username not available
	sf::Uint64 uuid;

	MatchInvitationInterrupt& matchInvitationInterrupt;
};

#endif