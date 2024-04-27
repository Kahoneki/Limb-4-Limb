#ifndef MAINMENU_H
#define MAINMENU_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>
#include "TextBox.h"
#include "Button.h"
#include "AccountManager.h"
#include <functional>

class SceneManager; //Forward declaration

class MainMenu : public BaseLevel
{
public:

	MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~MainMenu();


	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	SceneManager& sceneManager;
	
	bool mousePressedLastFrame;
	
	sf::RectangleShape background;
	
	sf::Font font;
	TextBox title;
	
	//Will only be displayed if player is logged in
	TextBox username;
	TextBox ranking;
	AccountManager& accountManager;

	Button local;
	Button online;
	Button registration;
	Button login;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onLocalButtonClick;
	std::function<void(void)> onOnlineButtonClick;
	std::function<void(void)> onRegistrationButtonClick;
	std::function<void(void)> onLoginButtonClick;
};

#endif