#ifndef LOGINSCREEN_H
#define LOGINSCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>

#include "InputBox.h"
#include "Button.h"


//Forward declarations
class SceneManager;

template<typename ParentType>
class NetworkListener;

class LoginScreen : public BaseLevel
{
public:

	LoginScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~LoginScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	//Will be called by network listener
	void setLoginStatus(bool val);
	void setRanking(int ranking);

private:
	SceneManager& sceneManager;

	sf::Font font;
	sf::RectangleShape background;

	bool mousePressedLastFrame;

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
	void sendAccountInfoToServer();
	NetworkListener<LoginScreen>* networkListener;
	int networkListenerIndex;

	//To be populated
	sf::Int8 loginStatus; //-1 = server hasn't responded yet, 0 = username available, 1 = username not available
	std::string username;
	sf::Uint64 uuid;
	sf::Int32 ranking;
};

#endif