#ifndef MAINMENU_H
#define MAINMENU_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>
#include "TextBox.h"
#include "Button.h"
#include <functional>


//Forward declarations
class SceneManager;
class AccountManager;
class MatchInvitationInterrupt;
class NetworkManager;


class MainMenu : public BaseLevel
{
public:

	MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sm);
	~MainMenu();


	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	SceneManager& sceneManager;
	MatchInvitationInterrupt& matchInvitationInterrupt;
	AccountManager& accountManager;
	NetworkManager& networkManager;
	
	sf::RectangleShape background;
	
	sf::Font font;
	TextBox title;
	//Will only be displayed if player is logged in
	TextBox username;
	TextBox ranking;
	//--//

	Button local;
	Button online;
	Button registration;
	Button login;
	Button quit;
	Button switchOnlineStatus; //Initially "Go Online". If clicked, "Go Offline"

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onLocalButtonClick;
	std::function<void(void)> onOnlineButtonClick;
	std::function<void(void)> onRegistrationButtonClick;
	std::function<void(void)> onLoginButtonClick;
	std::function<void(void)> onQuitClick;
	std::function<void(void)> onSwitchOnlineStatusButtonClick;
};

#endif