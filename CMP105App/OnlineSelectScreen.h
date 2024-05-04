#ifndef ONLINE_SELECT_SCREEN_H
#define ONLINE_SELECT_SCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>
#include "Button.h"
#include "AccountManager.h"
#include <functional>

class SceneManager; //Forward declaration

class OnlineSelectScreen : public BaseLevel
{
public:

	OnlineSelectScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~OnlineSelectScreen();


	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	SceneManager& sceneManager;

	sf::RectangleShape background;

	sf::Font font;
	TextBox title;

	//Will only be displayed if player is logged in
	TextBox username;
	TextBox ranking;
	AccountManager& accountManager;

	Button sendInvite;
	Button randomMatch;
	Button backButton;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onSendInviteButtonClick;
	std::function<void(void)> onRandomMatchButtonClick;
	std::function<void(void)> onBackButtonClick;
};

#endif