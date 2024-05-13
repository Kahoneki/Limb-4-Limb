#ifndef LOGINSCREEN_H
#define LOGINSCREEN_H

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

class LoginScreen : public BaseLevel
{
public:

	LoginScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~LoginScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	friend class NetworkListener<LoginScreen>;

private:
	SceneManager& sceneManager;
	NetworkManager& networkManager;

	sf::Font font;
	sf::RectangleShape background;

	InputBox usernameBox;

	Button loginButton; //To confirm registration
	Button backButton;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onLoginButtonClick;
	std::function<void(void)> onBackButtonClick;

	TextBox statusBar; //For displaying status to user - e.g.: connecting to server, generating uuid, username invalid, etc.
	bool displayStatusBar;

	bool checkClientSideUsernameValidity();
	int minUsernameLength;
	int maxUsernameLength;

	bool awaitServerResponses;
	NetworkListener<LoginScreen>* networkListener;
	int networkListenerIndex;

	//To be populated
	sf::Int8 loginStatus; //-1 = server hasn't responded yet, 0 = username available, 1 = username not available
	std::string username;
	sf::Uint64 uuid;
	sf::Int32 ranking;


	MatchInvitationInterrupt& matchInvitationInterrupt;
};

#endif