#pragma once

#include <SFML/Graphics.hpp>
#include "Input.h"
#include "AudioManager.h"
#include "GameState.h"
#include <string>
#include <iostream>

class BaseLevel
{
public:
	BaseLevel();
	/*BaseLevel(sf::RenderWindow* hwnd);
	BaseLevel(sf::RenderWindow* hwnd, Input* in);
	BaseLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs);
	BaseLevel(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud);*/
	virtual ~BaseLevel();

	virtual void handleInput(float dt) = 0;
	virtual void update(float dt) = 0;
	virtual void render() = 0;

protected:
	// Default functions for rendering to the screen.
	void beginDraw();
	void endDraw();

	// Default variables for level class.
	sf::RenderWindow* window;
	Input* input;
	GameState* gameState;
	AudioManager* audio;
};

