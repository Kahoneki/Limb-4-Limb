#include "Button.h"
#include <iostream>

Button::Button() {
}

Button::Button(sf::Vector2f pos, sf::Vector2f size, sf::Color inactiveFillCol, sf::Color activeFillCol, sf::Color txCol, float txSize, sf::Font& txFont, std::function<void(void)> onClickCallback, const char* defaultTxt)
	: TextBox(pos, size, inactiveFillCol, txCol, txSize, txFont, defaultTxt)
{
	inactiveFillColour = inactiveFillCol;
	activeFillColour = activeFillCol;
	onClick = onClickCallback;

	mousePressedLastFrame = true;
}

Button::Button(float posX, float posY, float sizeX, float sizeY, sf::Color inactiveFillCol, sf::Color activeFillCol, sf::Color txCol, float txSize, sf::Font& txFont, std::function<void(void)> onClickCallback, const char* defaultTxt)
	: TextBox(posX, posY, sizeX, sizeY, inactiveFillCol, txCol, txSize, txFont, defaultTxt)
{
	inactiveFillColour = inactiveFillCol;
	activeFillColour = activeFillCol;
	onClick = onClickCallback;

	mousePressedLastFrame = true;
}

void Button::processEvents(sf::Vector2f mousePos) {
	bool mouseOverBox{ box.getGlobalBounds().contains(mousePos) };

	if (mouseOverBox && box.getFillColor() == inactiveFillColour) {
		box.setFillColor(activeFillColour);
	}
	else if (!mouseOverBox && box.getFillColor() == activeFillColour) {
		box.setFillColor(inactiveFillColour);
	}

	if (mouseOverBox && sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame) {
		onClick();
	}

	mousePressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}
