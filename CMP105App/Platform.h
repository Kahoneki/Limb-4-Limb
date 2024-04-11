#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/Graphics.hpp>
#include "Framework/GameObject.h"

class Platform : public GameObject
{
public:
	Platform(float xPos, float yPos, float width, float height, bool passable);
	Platform();

	bool getPassable();
	void setPassable(bool val);

	inline bool operator==(const Platform& other) const {
		return (getPosition() == other.getPosition()) && (getSize() == other.getSize()) && (isPassable == other.isPassable);
	}

protected:
	bool isPassable;
};

#endif