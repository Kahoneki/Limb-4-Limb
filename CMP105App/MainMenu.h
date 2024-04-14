#ifndef MAINMENU_H
#define MAINMENU_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
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