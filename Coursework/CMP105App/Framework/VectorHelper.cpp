#include "VectorHelper.h"

// Returns a normalised vector
sf::Vector2f VectorHelper::normalise(const sf::Vector2f &source)
{
	float length = (float)sqrt((double(source.x) * double(source.x)) + (double(source.y) * double(source.y)));
	if (length != 0)
		return sf::Vector2f(source.x / length, source.y / length);
	else
		return source;
}

// Returns the Vector magnitude
float VectorHelper::magnitude(sf::Vector2f vec)
{
	return (float)sqrt((double(vec.x) * double(vec.x)) + (double(vec.y) * double(vec.y)));
}




