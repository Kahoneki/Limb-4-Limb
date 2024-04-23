#ifndef MAINMENU_H
#define MAINMENU_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>
#include "TextBox.h"
#include "Button.h"
#include <functional>

class SceneManager; //Forward declaration

class MainMenu : public BaseLevel
{
public:

	MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~MainMenu();

	void InitialiseCallbacks();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	SceneManager& sceneManager;
	
	bool mousePressedLastFrame;
	
	sf::RectangleShape background;
	
	sf::Font font;
	TextBox title;

	Button local;
	Button online;
	Button registration;
	Button login;

	//Callbacks
	std::function<void(void)> onLocalButtonClick;
	std::function<void(void)> onOnlineButtonClick;
	std::function<void(void)> onRegistrationButtonClick;
	std::function<void(void)> onLoginButtonClick;
};

#endif