#include "TextBox.h"
#include <iostream>

TextBox::TextBox() {}

TextBox::TextBox(sf::Vector2f pos, sf::Vector2f size, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, const char* txt, bool centreText) {
	box = sf::RectangleShape(size);
	box.setPosition(pos);
	box.setFillColor(fillCol);
	
	text = sf::Text(txt, txFont);
	text.setFillColor(txCol);
	text.setCharacterSize(txSize);

	if (centreText) {
		text.setPosition((pos.x+box.getSize().x/2) - (text.getGlobalBounds().width/2), pos.y);
	}
	else {
		text.setPosition(pos);
	}
	
}

TextBox::TextBox(float posX, float posY, float sizeX, float sizeY, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, const char* txt, bool centreText) {
	box = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
	box.setPosition(posX, posY);
	box.setFillColor(fillCol);

	text = sf::Text(txt, txFont);
	text.setFillColor(txCol);
	text.setCharacterSize(txSize);

	if (centreText) {
		text.setPosition((posX + box.getSize().x / 2) - (text.getGlobalBounds().width / 2), posY);
	}
	else {
		text.setPosition(posX, posY);
	}
}

void TextBox::RecentreText()
{
	text.setPosition((box.getPosition().x + box.getSize().x / 2) - (text.getGlobalBounds().width / 2), box.getPosition().y);
}

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(box, states);
	target.draw(text, states);
}