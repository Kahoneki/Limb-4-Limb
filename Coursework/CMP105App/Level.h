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
	
};