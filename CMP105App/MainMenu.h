#ifndef MAINMENU_H
#define MAINMENU_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/AudioManager.h"
#include "SceneManager.h"
#include <string.h>
#include <iostream>

class SceneManager; //Forward declaration

class MainMenu : public BaseLevel
{
public:

	MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~MainMenu();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	void InitialiseMainMenu();

private:
	SceneManager& sceneManager;
	
	bool mousePressedLastFrame;
	
	sf::Font font;
	sf::RectangleShape background;
	sf::RectangleShape titleBox;
	sf::RectangleShape localBox;
	sf::RectangleShape onlineBox;
	sf::Text titleText;
	sf::Text localText;
	sf::Text onlineText;
};

#endif