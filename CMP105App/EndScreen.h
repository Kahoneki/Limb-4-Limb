#ifndef ENDSCREEN_H
#define ENDSCREEN_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/AudioManager.h"
#include <functional>

#include "SceneManager.h"

class EndScreen : public BaseLevel
{
public:
	EndScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, bool sceneLocal, std::string resultText);
	~EndScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:

	SceneManager& sceneManager;

	sf::RectangleShape background;

	sf::Font font;
	TextBox result; //E.g.: "Player 1 Wins!"
	Button restart; //If local match
	Button exitToMainMenu;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> restartOnClick;
	std::function<void(void)> mainMenuOnClick;

	bool sceneIsLocal; //Coming from local match if true, coming from online match if false
};

#endif