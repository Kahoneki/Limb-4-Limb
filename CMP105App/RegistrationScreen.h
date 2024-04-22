#ifndef REGISTRATION_SCREEN_H
#define REGISTRATION_SCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>

#include "InputBox.h"

class SceneManager; //Forward declaration

class RegistrationScreen : public BaseLevel
{
public:

	RegistrationScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~RegistrationScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	SceneManager& sceneManager;

	sf::Font font;
	sf::RectangleShape background;
	
	InputBox usernameBox;

};

#endif