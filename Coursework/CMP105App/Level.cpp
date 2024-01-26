#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	// initialise game objects

}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{
	
}

// Update game objects
void Level::update(float dt)
{
	
}

// Render level
void Level::render()
{
	beginDraw();

	endDraw();
}

