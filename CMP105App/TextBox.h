#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "Framework/GameObject.h"


//Basic text box class that takes in position, size, fill colour, text colour, text size, text font, text

class TextBox : public GameObject
{
public:
	TextBox(); //Empty text box
	TextBox(sf::Vector2f pos, sf::Vector2f size, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, const char* txt);
	TextBox(float posX, float posY, float sizeX, float sizeY, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, const char* txt);

	sf::RectangleShape box;
	sf::Text text;


private:
	//Override pure virtual draw function
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif