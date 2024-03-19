#ifndef ENDSCREEN_H
#define ENDSCREEN_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/AudioManager.h"

#include "SceneManager.h"

class EndScreen : public BaseLevel
{
public:
	EndScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm, bool player1Win);
	~EndScreen();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

private:

	SceneManager& sceneManager;

	sf::Font font;
	sf::RectangleShape background;
	sf::RectangleShape winBox;
	sf::RectangleShape restartBox;
	sf::Text winText;
	sf::Text restartText;

	bool mousePressedLastFrame;
};

#endif