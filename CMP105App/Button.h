#ifndef BUTTON_H
#define BUTTON_H

#include "TextBox.h"
#include <functional>

#include <string>


class Button : public TextBox
{
public:
	Button(); //Empty button
	Button(sf::Vector2f pos, sf::Vector2f size, sf::Color inactiveFillCol, sf::Color activeFillCol, sf::Color txCol, float txSize, sf::Font& txFont, std::function<void(void)> onClickCallback, const char* defaultTxt = "");
	Button(float posX, float posY, float sizeX, float sizeY, sf::Color inactiveFillCol, sf::Color activeFillCol, sf::Color txCol, float txSize, sf::Font& txFont, std::function<void(void)> onClickCallback,  const char* defaultTxt = "");

	void processEvents(sf::Vector2f mousePos); //To be called every frame


private:
	sf::Color inactiveFillColour;
	sf::Color activeFillColour; //When hovering
	std::function<void(void)> onClick; //Callback
	
	bool mousePressedLastFrame; //Avoid check for mouse clicking on input box from passing every frame

};

#endif