#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/Graphics.hpp>
#include "Framework/GameObject.h"
#include <iostream>

class Platform : public GameObject
{
public:
	Platform(float xPos, float yPos, float width, float height, bool passable);
	Platform();

	bool getPassable();
	void setPassable(bool val);

protected:
	bool isPassable;
};

#endif