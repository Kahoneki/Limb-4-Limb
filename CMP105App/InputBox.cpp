#include "InputBox.h"
#include "Framework/Input.h"
#include <iostream>

bool charInCharacterSet(char character, const char* characterSet);


InputBox::InputBox() {}

InputBox::InputBox(sf::Vector2f pos, sf::Vector2f size, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, int maxLen, const char* acceptableChars, const char* defaultTxt, bool asterisks, sf::Color activeCol)
	: TextBox(pos, size, fillCol, txCol, txSize, txFont, defaultTxt)
{
	maxLength = maxLen;
	acceptableCharacters = acceptableChars;
	defaultText = defaultTxt;
	displayText = defaultTxt;
	displayAsAsterisks = asterisks;
	activeColour = activeCol == sf::Color::Transparent ? fillCol : activeCol;
	inactiveColour = fillCol;
	defaultWidth = size.x;
	active = false;

	backspaceCooldown = 0.05f;
	backspaceCooldownTimer = 0.0f;

	mousePressedLastFrame = true;
}

InputBox::InputBox(float posX, float posY, float sizeX, float sizeY, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, int maxLen, const char* acceptableChars, const char* defaultTxt, bool asterisks, sf::Color activeCol)
	: TextBox(posX, posY, sizeX, sizeY, fillCol, txCol, txSize, txFont, defaultTxt)
{
	maxLength = maxLen;
	acceptableCharacters = acceptableChars;
	defaultText = defaultTxt;
	displayText = defaultTxt;
	displayAsAsterisks = asterisks;
	activeColour = activeCol == sf::Color::Transparent ? fillCol : activeCol;
	inactiveColour = fillCol;
	defaultWidth = sizeX;
	active = false;

	backspaceCooldown = 0.05f;
	backspaceCooldownTimer = 0.0f;

	mousePressedLastFrame = true;
}



void InputBox::processEvents(float dt, sf::Vector2f mousePos) {
	bool mouseOverBox{ box.getGlobalBounds().contains(mousePos) };
	if (mouseOverBox && sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame) {
		active = true;
		if (displayAsAsterisks) {
			displayText = "";
			for (char c : typedText) {
				displayText.push_back('*');
			}
		}
		else {
			displayText = typedText;
		}
		displayText.push_back('|');
		sf::Text tempText{ displayText, *text.getFont() };
		box.setSize(sf::Vector2f(std::max(tempText.getGlobalBounds().width + 20, defaultWidth), box.getSize().y));
	}
	else if (!mouseOverBox && sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame && active) {
		active = false;

		box.setSize(sf::Vector2f(defaultWidth, box.getSize().y));
		sf::Text tempText{"", *text.getFont()};
		std::string tempString{""};
		for (int i{ 0 }; i < typedText.length(); ++i) {
			tempString.push_back(displayAsAsterisks ? '*' : typedText.at(i));
			tempText.setString(tempString);
			if ((tempText.getGlobalBounds().width + 20) > defaultWidth) {
				tempString.pop_back();
				break;
			}
		}
		displayText = tempString;

		if (displayText.length() == 0) {
			displayText = defaultText;
		}
		
	}
	box.setFillColor(active ? activeColour : inactiveColour);

	if (active) {
		if (input->getTextEntered()) {
			char character = static_cast<char>(input->getText());
			if (typedText.length() < maxLength) {
				if (charInCharacterSet(character, acceptableCharacters)) {
					typedText.push_back(character);
					displayText.pop_back();
					displayText.push_back(displayAsAsterisks ? '*' : character);
					displayText.push_back('|');
					box.setSize(sf::Vector2f(std::max(defaultWidth, text.getGlobalBounds().width + 20), box.getSize().y));
				}
			}
		}
		else if (input->isKeyDown(sf::Keyboard::Backspace) && displayText != "|" && backspaceCooldownTimer <= 0) {
			backspaceCooldownTimer = backspaceCooldown;
			typedText.pop_back();
			displayText.pop_back();
			displayText.pop_back();
			displayText.push_back('|');
			box.setSize(sf::Vector2f(std::max(defaultWidth, text.getGlobalBounds().width + 20), box.getSize().y));
		}
	}

	text.setString(displayText);
	
	backspaceCooldownTimer -= dt;
	mousePressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

bool charInCharacterSet(char character, const char* characterSet) {
	//Loop through all characters in acceptable character list to see if provided character matches any of them (incr pointer until null pointer is reached)
	for (char c{ *characterSet }; c != '\0'; c = *++characterSet) {
		if (character == c) { return true; }
	}
	//No match found
	return false;
}



std::string InputBox::getTypedText() { return typedText; }