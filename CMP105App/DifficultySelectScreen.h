#ifndef DIFFICULTY_SELECT_SCREEN_H
#define DIFFICULTY_SELECT_SCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>

#include "InputBox.h"
#include "Button.h"


//Forward declarations
class SceneManager;
class MatchInvitationInterrupt;

class DifficultySelectScreen : public BaseLevel
{
public:

	DifficultySelectScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~DifficultySelectScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:
	SceneManager& sceneManager;

	sf::Font font;
	sf::RectangleShape background;

	TextBox selectDifficulty;

	//Difficulties
	Button one;
	Button two;
	Button three;
	Button four;
	Button five;

	sf::RectangleShape selectionBorder;
	
	Button start;

	Button backButton;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> oneOnClick;
	std::function<void(void)> twoOnClick;
	std::function<void(void)> threeOnClick;
	std::function<void(void)> fourOnClick;
	std::function<void(void)> fiveOnClick;
	std::function<void(void)> startOnClick;
	std::function<void(void)> backButtonOnClick;

	int difficulty;

	MatchInvitationInterrupt& matchInvitationInterrupt;
};

#endif