#include "TextBox.h"
#include <iostream>

TextBox::TextBox() {}

TextBox::TextBox(sf::Vector2f pos, sf::Vector2f size, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, const char* txt) {
	box = sf::RectangleShape(size);
	box.setPosition(pos);
	box.setFillColor(fillCol);
	
	text = sf::Text(txt, txFont);
	text.setPosition(pos);
	text.setFillColor(txCol);
	text.setCharacterSize(txSize);
	
}

TextBox::TextBox(float posX, float posY, float sizeX, float sizeY, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, const char* txt) {
	box = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
	box.setPosition(posX, posY);
	box.setFillColor(fillCol);

	text = sf::Text(txt, txFont);
	text.setPosition(posX, posY);
	text.setFillColor(txCol);
	text.setCharacterSize(txSize);
}

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(box, states);
	target.draw(text, states);
}