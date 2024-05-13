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
	EndScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, int sceneType, std::string resultText, int difficulty=-1);
	~EndScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:

	SceneManager& sceneManager;

	sf::RectangleShape background;

	sf::Font font;
	TextBox result; //E.g.: "Player 1 Wins!"
	Button restart; //If singleplayer match or local match
	Button exitToMainMenu;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> restartOnClick;
	std::function<void(void)> mainMenuOnClick;

	int scene; //Scene that user came from. 0 = singleplayer, 1 = local, 2 = online
	int difficultySetting; //If user came from a singleplayer match, this is the difficulty of the AI, otherwise, it's unused
};

#endif