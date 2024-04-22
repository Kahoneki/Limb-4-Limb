#ifndef MAINMENU_H
#define MAINMENU_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>
#include "TextBox.h"

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
	TextBox local;
	TextBox online;
	TextBox registration;
	TextBox login;
};

#endif