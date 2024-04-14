#include "Platform.h"

Platform::Platform() {}

Platform::Platform(float xPos, float yPos, float width, float height, bool passable)
{
	setPosition(sf::Vector2f(xPos, yPos));
	setSize(sf::Vector2f(width, height));
	isPassable = passable;
}

bool Platform::getPassable() { return isPassable; }

void Platform::setPassable(bool val) { isPassable = val; }