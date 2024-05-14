#ifndef RANDOM_MATCH_SCREEN_H
#define RANDOM_MATCH_SCREEN_H

#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include <iostream>

#include "InputBox.h"
#include "Button.h"




//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------THIS CLASS IS UNUSED AND UNFINISHED--------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------------------------------------------------//




//Forward declarations
class SceneManager;

template<typename ParentType>
class NetworkListener;

class RandomMatchScreen : public BaseLevel
{
public:

	RandomMatchScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~RandomMatchScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;


private:
	SceneManager& sceneManager;

	sf::Font font;
	sf::RectangleShape background;

	Button backButton;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> onBackButtonClick;

	TextBox statusBar; //For displaying status to user - e.g.: connecting to server, searching for user, awaiting response from user, etc.
	bool displayStatusBar;

	bool awaitServerResponses;
	NetworkListener<RandomMatchScreen>* networkListener;
	int networkListenerIndex;
};

#endif