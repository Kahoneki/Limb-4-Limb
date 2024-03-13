#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"


class Level : BaseLevel{
public:
	Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);
	~Level();

	void handleInput(float dt) override;
	void update(float dt);
	void render();

private:
	// Default functions for rendering to the screen.
	

	// Default variables for level class.
	//Player robot;
	//sf::Texture robotTexture;
	sf::RectangleShape titlebox;
	sf::RectangleShape opt1box;
	sf::RectangleShape opt2box;
	sf::RectangleShape opt3box;
	sf::RectangleShape background;
	sf::Font font;
	sf::Text title;
	sf::Text opt1txt;
	sf::Text opt2txt;
	sf::Text opt3txt;
};